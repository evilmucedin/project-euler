// A more complete calculator UI using GTK+ and simple stateful logic.
#include <gtk/gtk.h>
#include <string>
#include <cmath>

static GtkWidget *entry;
static double stored_value = 0.0;
static char pending_op = '\0';
static bool entering = false; // whether user is typing a number

// Helpers
static std::string get_entry_text() {
    const gchar *t = gtk_entry_get_text(GTK_ENTRY(entry));
    return t ? std::string(t) : std::string();
}

static void set_entry_text(const std::string &s) {
    gtk_entry_set_text(GTK_ENTRY(entry), s.c_str());
}

static double to_double(const std::string &s) {
    try {
        return std::stod(s);
    } catch (...) {
        return 0.0;
    }
}

static void apply_pending_op(double rhs) {
    if (pending_op == '+') stored_value = stored_value + rhs;
    else if (pending_op == '-') stored_value = stored_value - rhs;
    else if (pending_op == '*') stored_value = stored_value * rhs;
    else if (pending_op == '/') stored_value = (rhs == 0.0) ? NAN : stored_value / rhs;
    else stored_value = rhs; // no pending op
}

// Callbacks
extern "C" void on_digit_clicked(GtkWidget *widget, gpointer data) {
    const char *digit = (const char*)data;
    std::string text = get_entry_text();
    if (!entering || text == "0") {
        text = digit;
        entering = true;
    } else {
        text += digit;
    }
    set_entry_text(text);
}

extern "C" void on_dot_clicked(GtkWidget *widget, gpointer) {
    std::string text = get_entry_text();
    if (!entering) { text = "0."; entering = true; }
    else if (text.find('.') == std::string::npos) text += '.';
    set_entry_text(text);
}

extern "C" void on_clear_clicked(GtkWidget*, gpointer) {
    set_entry_text("0");
    stored_value = 0.0;
    pending_op = '\0';
    entering = false;
}

extern "C" void on_backspace_clicked(GtkWidget*, gpointer) {
    std::string text = get_entry_text();
    if (text.size() <= 1) {
        set_entry_text("0");
        entering = false;
    } else {
        text.pop_back();
        set_entry_text(text);
    }
}

extern "C" void on_plusminus_clicked(GtkWidget*, gpointer) {
    std::string text = get_entry_text();
    if (text.size() == 0 || text == "0") return;
    if (text[0] == '-') text.erase(0,1); else text = "-" + text;
    set_entry_text(text);
}

extern "C" void on_op_clicked(GtkWidget *widget, gpointer data) {
    char op = *(char*)data;
    double current = to_double(get_entry_text());
    if (pending_op != '\0' && entering) {
        apply_pending_op(current);
    } else if (!entering) {
        // user pressed operator twice; change pending op
    } else {
        stored_value = current;
    }
    pending_op = op;
    entering = false;
    if (std::isnan(stored_value)) set_entry_text("Error");
    else {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", stored_value);
        set_entry_text(buf);
    }
}

extern "C" void on_equals_clicked(GtkWidget*, gpointer) {
    double current = to_double(get_entry_text());
    if (pending_op != '\0') apply_pending_op(current);
    pending_op = '\0';
    entering = false;
    if (std::isnan(stored_value)) set_entry_text("Error");
    else {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", stored_value);
        set_entry_text(buf);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    gtk_container_set_border_width(GTK_CONTAINER(window), 8);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(entry), 1.0); // right-align
    gtk_entry_set_text(GTK_ENTRY(entry), "0");
    gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_vexpand(entry, FALSE);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    auto add_button = [&](const char *label, int col, int row, GCallback cb, gpointer data, int w=1) {
        GtkWidget *b = gtk_button_new_with_label(label);
        gtk_widget_set_hexpand(b, TRUE);
        gtk_widget_set_vexpand(b, TRUE);
        g_signal_connect(b, "clicked", cb, data);
        gtk_grid_attach(GTK_GRID(grid), b, col, row, w, 1);
    };

    // First row: clear, backspace, +/- , divide
    add_button("C", 0, 1, G_CALLBACK(on_clear_clicked), NULL);
    add_button("⌫", 1, 1, G_CALLBACK(on_backspace_clicked), NULL);
    add_button("±", 2, 1, G_CALLBACK(on_plusminus_clicked), NULL);
    char op_div = '/'; add_button("/", 3, 1, G_CALLBACK(on_op_clicked), &op_div);

    // Digits and ops
    char op_mul = '*';
    add_button("7", 0, 2, G_CALLBACK(on_digit_clicked), (gpointer)"7");
    add_button("8", 1, 2, G_CALLBACK(on_digit_clicked), (gpointer)"8");
    add_button("9", 2, 2, G_CALLBACK(on_digit_clicked), (gpointer)"9");
    add_button("*", 3, 2, G_CALLBACK(on_op_clicked), &op_mul);

    char op_sub = '-';
    add_button("4", 0, 3, G_CALLBACK(on_digit_clicked), (gpointer)"4");
    add_button("5", 1, 3, G_CALLBACK(on_digit_clicked), (gpointer)"5");
    add_button("6", 2, 3, G_CALLBACK(on_digit_clicked), (gpointer)"6");
    add_button("-", 3, 3, G_CALLBACK(on_op_clicked), &op_sub);

    char op_add = '+';
    add_button("1", 0, 4, G_CALLBACK(on_digit_clicked), (gpointer)"1");
    add_button("2", 1, 4, G_CALLBACK(on_digit_clicked), (gpointer)"2");
    add_button("3", 2, 4, G_CALLBACK(on_digit_clicked), (gpointer)"3");
    add_button("+", 3, 4, G_CALLBACK(on_op_clicked), &op_add);

    // Last row: 0 (span 2), dot, equals
    add_button("0", 0, 5, G_CALLBACK(on_digit_clicked), (gpointer)"0", 2);
    add_button(".", 2, 5, G_CALLBACK(on_dot_clicked), NULL);
    add_button("=", 3, 5, G_CALLBACK(on_equals_clicked), NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
