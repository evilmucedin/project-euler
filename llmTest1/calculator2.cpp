#include <gtk/gtk.h>

// Function prototypes for button click handlers
void on_add_clicked(GtkWidget *widget, gpointer data);
void on_subtract_clicked(GtkWidget *widget, gpointer data);
void on_multiply_clicked(GtkWidget *widget, gpointer data);
void on_divide_clicked(GtkWidget *widget, gpointer data);

// Global variables to store the numbers and display result
double num1 = 0.0;
double num2 = 0.0;
GtkEntry *entry;

int main(int argc, char *argv[]) {
    // Initialize GTK+
    gtk_init(&argc, &argv);

    // Create a new window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Calculator");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Entry widget to display results and take inputs
    entry = GTK_ENTRY(gtk_entry_new());
    gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE); // Ensure the entry is editable

    gtk_entry_set_text(entry, "0");
    gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE); // Make it editable for user input
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(entry), TRUE, TRUE, 0);

    // Horizontal box container for buttons
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    // Create and add buttons to the horizontal box
    GtkWidget *add_button = gtk_button_new_with_label("+");
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), add_button, TRUE, TRUE, 0);

    GtkWidget *subtract_button = gtk_button_new_with_label("-");
    g_signal_connect(subtract_button, "clicked", G_CALLBACK(on_subtract_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), subtract_button, TRUE, TRUE, 0);

    GtkWidget *multiply_button = gtk_button_new_with_label("*");
    g_signal_connect(multiply_button, "clicked", G_CALLBACK(on_multiply_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), multiply_button, TRUE, TRUE, 0);

    GtkWidget *divide_button = gtk_button_new_with_label("/");
    g_signal_connect(divide_button, "clicked", G_CALLBACK(on_divide_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), divide_button, TRUE, TRUE, 0);

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}

// Button click handler for addition
void on_add_clicked(GtkWidget *widget, gpointer data) {
    num2 = g_strtod(gtk_entry_get_text(entry), NULL);
    num1 += num2;
    gchar result[50];
    snprintf(result, sizeof(result), "%.2f", num1);
    gtk_entry_set_text(entry, result);
}

// Button click handler for subtraction
void on_subtract_clicked(GtkWidget *widget, gpointer data) {
    num2 = g_strtod(gtk_entry_get_text(entry), NULL);
    num1 -= num2;
    gchar result[50];
    snprintf(result, sizeof(result), "%.2f", num1);
    gtk_entry_set_text(entry, result);
}

// Button click handler for multiplication
void on_multiply_clicked(GtkWidget *widget, gpointer data) {
    num2 = g_strtod(gtk_entry_get_text(entry), NULL);
    num1 *= num2;
    gchar result[50];
    snprintf(result, sizeof(result), "%.2f", num1);
    gtk_entry_set_text(entry, result);
}

// Button click handler for division
void on_divide_clicked(GtkWidget *widget, gpointer data) {
    num2 = g_strtod(gtk_entry_get_text(entry), NULL);
    if (num2 != 0) {
        num1 /= num2;
    } else {
        gtk_entry_set_text(entry, "Error");
    }
    gchar result[50];
    snprintf(result, sizeof(result), "%.2f", num1);
    gtk_entry_set_text(entry, result);
}
