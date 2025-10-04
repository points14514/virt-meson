#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

// 虚拟机配置结构体
typedef struct {
    char *interface;
    char *iso_path;
    char *img_path;
    char *cpu_arch;
    gboolean has_gpu;
    gboolean has_fpu;
    char *vm_name;
    gboolean has_netcard;
    gboolean has_soundcard;
    gboolean has_graphiccard;
} VMConfig;

// 全局变量
GtkWidget *main_window;
GtkWidget *vm_list; // 虚拟机列表

// 创建新虚拟机对话框
static void create_new_vm(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *content_area, *grid;
    GtkWidget *label, *entry;
    GtkWidget *check_button;
    GtkWidget *ok_button, *cancel_button;
    
    // 创建对话框
    dialog = gtk_dialog_new_with_buttons("新建虚拟机",
                                        GTK_WINDOW(main_window),
                                        GTK_DIALOG_MODAL,
                                        "_OK",
                                        GTK_RESPONSE_ACCEPT,
                                        "_Cancel",
                                        GTK_RESPONSE_REJECT,
                                        NULL);
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(content_area), grid);
    
    // 接口选择
    label = gtk_label_new("虚拟机接口:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    
    GtkWidget *combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "KVM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "QEMU");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "VirtualBox");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "VMware");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 0, 1, 1);
    
    // ISO镜像路径
    label = gtk_label_new("ISO镜像路径 (可选):");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    
    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    GtkWidget *iso_entry = entry;
    
    // IMG镜像路径
    label = gtk_label_new("IMG镜像路径 (可选):");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    
    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 2, 1, 1);
    GtkWidget *img_entry = entry;
    
    // CPU架构
    label = gtk_label_new("CPU架构:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    
    GtkWidget *cpu_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cpu_combo), "x86_64");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cpu_combo), "i386");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cpu_combo), "ARM");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cpu_combo), "ARM64");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cpu_combo), "RISC-V");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cpu_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), cpu_combo, 1, 3, 1, 1);
    
    // GPU支持
    check_button = gtk_check_button_new_with_label("需要GPU");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), TRUE);
    gtk_grid_attach(GTK_GRID(grid), check_button, 0, 4, 2, 1);
    GtkWidget *gpu_check = check_button;
    
    // FPU支持
    check_button = gtk_check_button_new_with_label("需要FPU");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), TRUE);
    gtk_grid_attach(GTK_GRID(grid), check_button, 0, 5, 2, 1);
    GtkWidget *fpu_check = check_button;
    
    // 虚拟机名称
    label = gtk_label_new("虚拟机名称 (可选):");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 6, 1, 1);
    
    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 6, 1, 1);
    GtkWidget *name_entry = entry;
    
    // 附加选项
    label = gtk_label_new("附加选项:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 7, 1, 1);
    
    check_button = gtk_check_button_new_with_label("需要网卡");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), TRUE);
    gtk_grid_attach(GTK_GRID(grid), check_button, 0, 8, 1, 1);
    GtkWidget *net_check = check_button;
    
    check_button = gtk_check_button_new_with_label("需要声卡");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), FALSE);
    gtk_grid_attach(GTK_GRID(grid), check_button, 1, 8, 1, 1);
    GtkWidget *sound_check = check_button;
    
    check_button = gtk_check_button_new_with_label("需要显卡");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button), FALSE);
    gtk_grid_attach(GTK_GRID(grid), check_button, 0, 9, 1, 1);
    GtkWidget *graphic_check = check_button;
    
    gtk_widget_show_all(dialog);
    
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (result == GTK_RESPONSE_ACCEPT) {
        VMConfig *config = g_new(VMConfig, 1);
        
        // 获取接口选择
        gchar *interface_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
        config->interface = g_strdup(interface_text);
        g_free(interface_text);
        
        // 获取ISO路径
        config->iso_path = g_strdup(gtk_entry_get_text(GTK_ENTRY(iso_entry)));
        
        // 获取IMG路径
        config->img_path = g_strdup(gtk_entry_get_text(GTK_ENTRY(img_entry)));
        
        // 获取CPU架构
        gchar *cpu_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cpu_combo));
        config->cpu_arch = g_strdup(cpu_text);
        g_free(cpu_text);
        
        // 获取GPU/FPU选项
        config->has_gpu = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gpu_check));
        config->has_fpu = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fpu_check));
        
        // 获取虚拟机名称
        const gchar *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        if (name && *name) {
            config->vm_name = g_strdup(name);
        } else {
            // 生成随机名称
            const char *packages[] = {"firefox", "libreoffice", "gimp", "vlc", "thunderbird", 
                                     "gcc", "python", "bash", "openssh", "apache"};
            srand(time(NULL));
            int index = rand() % (sizeof(packages) / sizeof(packages);
            config->vm_name = g_strdup_printf("%s-vm-%d", packages[index], rand() % 1000);
        }
        
        // 获取附加选项
        config->has_netcard = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(net_check));
        config->has_soundcard = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sound_check));
        config->has_graphiccard = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(graphic_check));
        
        // 添加到虚拟机列表
        GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(vm_list)));
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, config->vm_name, -1);
        
        // 显示创建成功消息
        GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                                          GTK_DIALOG_MODAL,
                                                          GTK_MESSAGE_INFO,
                                                          GTK_BUTTONS_OK,
                                                          "虚拟机 '%s' 创建成功!",
                                                          config->vm_name);
        gtk_dialog_run(GTK_DIALOG(message_dialog));
        gtk_widget_destroy(message_dialog);
        
        // 释放配置内存
        g_free(config->interface);
        g_free(config->iso_path);
        g_free(config->img_path);
        g_free(config->cpu_arch);
        g_free(config->vm_name);
        g_free(config);
    }
    
    gtk_widget_destroy(dialog);
}

// 启动虚拟机
static void start_vm(GtkWidget *widget, gpointer data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(vm_list));
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *vm_name;
        gtk_tree_model_get(model, &iter, 0, &vm_name, -1);
        
        // 显示启动提示
        GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                                         GTK_DIALOG_MODAL,
                                                         GTK_MESSAGE_INFO,
                                                         GTK_BUTTONS_OK,
                                                         "虚拟机 '%s' 马上启动，请耐心等待...",
                                                         vm_name);
        gtk_dialog_run(GTK_DIALOG(message_dialog));
        gtk_widget_destroy(message_dialog);
        
        // 模拟启动虚拟机
        GtkWidget *vm_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(vm_window), vm_name);
        gtk_window_set_default_size(GTK_WINDOW(vm_window), 800, 600);
        
        GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(vm_window), scrolled);
        
        GtkWidget *text_view = gtk_text_view_new();
        gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
        gtk_container_add(GTK_CONTAINER(scrolled), text_view);
        
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        gtk_text_buffer_set_text(buffer, "虚拟机启动中...\n", -1);
        
        // 模拟启动过程
        g_timeout_add(1000, (GSourceFunc)append_text, buffer);
        
        g_signal_connect(vm_window, "destroy", G_CALLBACK(gtk_widget_destroyed), &vm_window);
        gtk_widget_show_all(vm_window);
        
        g_free(vm_name);
    } else {
        GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                                          GTK_DIALOG_MODAL,
                                                          GTK_MESSAGE_WARNING,
                                                          GTK_BUTTONS_OK,
                                                          "请先选择一个虚拟机!");
        gtk_dialog_run(GTK_DIALOG(message_dialog));
        gtk_widget_destroy(message_dialog);
    }
}

// 模拟虚拟机启动过程
static gboolean append_text(GtkTextBuffer *buffer) {
    static int step = 0;
    const char *messages[] = {
        "检测硬件...\n",
        "初始化CPU...\n",
        "加载内存...\n",
        "检测存储设备...\n",
        "启动引导加载程序...\n",
        "加载操作系统...\n"
    };
    
    if (step < sizeof(messages) / sizeof(messages) {
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, messages[step], -1);
        step++;
        return TRUE;
    } else {
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(buffer, &iter);
        gtk_text_buffer_insert(buffer, &iter, "\n启动完成!\n", -1);
        return FALSE;
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *button;
    GtkWidget *box;
    GtkWidget *scrolled;
    GtkListStore *store;
    GtkWidget *tree_view;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    
    gtk_init(&argc, &argv);
    
    // 创建主窗口
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Virt-Meson 虚拟机管理器");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 600, 400);
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // 创建垂直布局盒子
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(main_window), box);
    
    // 创建按钮盒子
    GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(box), button_box, FALSE, FALSE, 0);
    
    // 新建虚拟机按钮
    button = gtk_button_new_with_label("新建虚拟机");
    g_signal_connect(button, "clicked", G_CALLBACK(create_new_vm), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    // 启动虚拟机按钮
    button = gtk_button_new_with_label("启动虚拟机");
    g_signal_connect(button, "clicked", G_CALLBACK(start_vm), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    // 创建虚拟机列表
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(box), scrolled, TRUE, TRUE, 0);
    
    store = gtk_list_store_new(1, G_TYPE_STRING);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("虚拟机名称", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    vm_list = tree_view;
    
    // 显示所有控件
    gtk_widget_show_all(main_window);
    
    // 进入主循环
    gtk_main();
    
    return 0;
}
