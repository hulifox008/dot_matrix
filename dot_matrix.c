#include <gtk/gtk.h>
#include <stdio.h>

#define RADIUS  20
#define WIDTH   ((RADIUS+2)*2*8)
#define HEIGHT  ((RADIUS+2)*2*8)

static cairo_surface_t *surface = NULL;

int cur_x = 1;

static void clear_surface(void)
{
    cairo_t *cr;

    cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    cairo_destroy(cr);
}

gboolean timer_cb(gpointer data)
{
    cur_x = (cur_x+1)%8;
    gtk_widget_queue_draw(data);
    return TRUE;
}

static gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    if(surface) {
        cairo_surface_destroy(surface);
    }

    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, gtk_widget_get_allocated_width(widget), gtk_widget_get_allocated_height(widget));
    clear_surface();

    g_timeout_add(300, timer_cb, widget);

    return TRUE;
}

static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    int x, y;
    /*
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
    */

    for(x = 0;x<8;x++) {
        for(y=0;y<8;y++) {
            if(x==cur_x)
                cairo_set_source_rgb(cr, 1, 0, 0);
            else
                cairo_set_source_rgb(cr, 0.8, 0.8, 0.8);
                
            cairo_arc(cr, (x+1)*(2*RADIUS+2), (y+1)*(2*RADIUS+2), RADIUS, 0, 2*G_PI);
            cairo_fill(cr);
        }
    }

    return FALSE;
}

static void close_window(void)
{
    if(surface) {
        cairo_surface_destroy(surface);
    }
}

static void activate(GtkApplication *app, gpointer user_data)
{
    fprintf(stderr, "activate\n");

    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *drawing_area;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Dot Matrix");

    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    gtk_container_set_border_width(GTK_CONTAINER(window), 8);

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(window), frame);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, WIDTH+16, HEIGHT+16);
    gtk_container_add(GTK_CONTAINER(frame), drawing_area);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_cb), NULL);
    g_signal_connect(drawing_area , "configure-event", G_CALLBACK(configure_event_cb), NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char* argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("fox.dot_matrix", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
    
}
