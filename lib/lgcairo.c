/* lgcairo.c
 * ---------------------------------------------------------------
 * Test glg Line Graph Widget in a GtkWindow
 *
 * (c) 2007, 2016 James Scott Jr
 * Date: 5/2016
 *
 * Authors:
 *   James Scott Jr <skoona@gmail.com>
 *
 * (c) 2023, 2023 weilinfox
 * Date: 1/2023
 *
 * Contributors:
 *   weilinfox <weilinfox@inuyasha.love>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>

#include "glg_cairo.h"


static gint gb_stop = FALSE;

void fn_show_point_selected (GtkWidget *graph, double x_value,     double y_value, 
												  double point_y_pos, double mouse_y_pos, gpointer data)
{
	g_print ("fn_show_point_selected: x_value=%1.0lf, y_value=%1.1lf, y_pos=%1.1lf, mouse_y_pos=%1.1lf, ranking=%1.1lf\n",
			  x_value, y_value, point_y_pos, mouse_y_pos, 
			  (point_y_pos < mouse_y_pos) ? ((point_y_pos / mouse_y_pos) * 100) : (( mouse_y_pos / point_y_pos) * 100) ); 
	return;
}
/*
 * Test Update
 *
 */
static gint fn_add_series_data_values (GlgLineGraph *graph)
{
	if ( gb_stop ) return FALSE;

    g_return_val_if_fail (graph != NULL, TRUE);

    if(GTK_IS_WIDGET(graph) == FALSE) {
        return TRUE;
    }


    glg_line_graph_data_series_add_value (graph, 0, g_random_double_range (5.0, 20.0) );
    glg_line_graph_data_series_add_value (graph, 1, g_random_double_range (20.0, 30.0) );
    glg_line_graph_data_series_add_value (graph, 2, 82.7 );
    glg_line_graph_data_series_add_value (graph, 3, g_random_double_range (30.0, 82.0) );
    glg_line_graph_data_series_add_value (graph, 4, g_random_double_range (98.0, 100.0) );

    glg_line_graph_redraw ( graph );

    return (TRUE);
}


/*
 * to enable debug messages
 * $ export G_MESSAGES_DEBUG=all
 */
int main (int argc, char **argv)
{
	GtkWidget *window = NULL;
	GlgLineGraph *glg;
	guint  ui_add_values = 0; 

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW(window), "GlgLineGraph Widget Example"); 
	gtk_window_set_default_size (GTK_WINDOW(window), GLG_USER_MODEL_X, GLG_USER_MODEL_Y);
	g_signal_connect (window, "destroy",
					  G_CALLBACK (gtk_main_quit), NULL);

        /*
         * IMPORTANT
         * set chart x and y increments and ranges
         * it is important to make tick-minor-x as small as possible or 1
         * and to also limit the scale-major-x to a value less than 100
         * otherwise the chart gets too busy.
         *
         * Also can only be set once, recreate graph widget to change this value
         */

	/*
	 * Create Graph -- Alternate Init Method
	 * and set all its options in one call */
	glg = glg_line_graph_new ("chart-set-elements", GLG_TOOLTIP | GLG_TITLE_T | GLG_TITLE_X | GLG_TITLE_Y | GLG_GRID_MAJOR_X |  GLG_GRID_MAJOR_Y | GLG_GRID_MINOR_X |  GLG_GRID_MINOR_Y | GLG_GRID_LABELS_X | GLG_GRID_LABELS_Y,
			"range-tick-minor-x", 1,
			"range-tick-major-x", 10,
			"range-scale-minor-x", 0,
			"range-scale-major-x", 40,
			"range-tick-minor-y", 2,
			"range-tick-major-y", 10,
			"range-scale-minor-y", 0,
			"range-scale-major-y", 110,
			"series-line-width", 2,
			"graph-title-foreground",  "blue",
			"graph-scale-foreground",  "black",
			"graph-chart-background",  "light blue",
			"graph-window-background", "white",
			"text-title-main", "<big><b>A GTK Line Graph Widget</b></big>\n<span foreground=\"orange\"><b><i>using Cairo Graphics</i></b></span>",
			"text-title-yaxis", "Pango Text Markup\n<small>is supported in all <span foreground=\"red\">X &amp; Y</span> titles.</small>",
			"text-title-xaxis", "<i>Click mouse button 1 to <span foreground=\"red\">toggle</span> popup legend.</i>",
			NULL);

	gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET(glg));
          

	/*
	 * setup to catch tooltip drill downs 
	 */
    g_signal_connect (glg, "point-selected", G_CALLBACK(fn_show_point_selected),NULL);

 /* 
  * Add empty data series containers, then add data later via timer routine 
  * this routine returns an int which is the series-number for plotting data
  * it should be retained and used with the glg_line_graph_data_series_add_value() api
  * the values are returned in sequence from 0 to N-series */

    glg_line_graph_data_series_add (glg, "Volts", "red");
    glg_line_graph_data_series_add (glg, "Battery", "blue");
    glg_line_graph_data_series_add (glg, "Load", "green");
    glg_line_graph_data_series_add (glg, "Charge", "orange");
    glg_line_graph_data_series_add (glg, "Line", "yellow");

	ui_add_values = g_timeout_add (1000, (GSourceFunc) fn_add_series_data_values, glg);

	gtk_widget_show_all (window);

	gtk_main ();
	
	gb_stop = TRUE;
	
	g_source_remove (ui_add_values );
	 
	return 0;
}
