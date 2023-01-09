package main

import (
	"errors"
	"fmt"
	"log"
	"os"
	"unsafe"

	"github.com/gotk3/gotk3/glib"
	"github.com/gotk3/gotk3/gtk"
)

/*
#cgo pkg-config: gtk+-3.0
#include <gtk/gtk.h>
#include "glg_cairo.h"

GlgLineGraph * test_glg_line_graph_new()
{
    return glg_line_graph_new ("chart-set-elements", GLG_TOOLTIP | GLG_TITLE_T | GLG_TITLE_X | GLG_TITLE_Y | GLG_GRID_MAJOR_X |  GLG_GRID_MAJOR_Y | GLG_GRID_MINOR_X |  GLG_GRID_MINOR_Y | GLG_GRID_LABELS_X | GLG_GRID_LABELS_Y,
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
}

void test_glg_line_graph_data_series_add(GlgLineGraph *glg)
{
    glg_line_graph_data_series_add (glg, "Volts", "red");
    glg_line_graph_data_series_add (glg, "Battery", "blue");
    glg_line_graph_data_series_add (glg, "Load", "green");
    glg_line_graph_data_series_add (glg, "Charge", "orange");
    glg_line_graph_data_series_add (glg, "Line", "yellow");
}
*/
import "C"

type GlgLineGraph struct {
	gtk.Bin
}

var gbStop = false
var glg *GlgLineGraph

func glgLineGraphNew() (*GlgLineGraph, error) {
	glg := C.test_glg_line_graph_new()
	if glg == nil {
		return nil, errors.New("cgo returned unexpected nil pointer")
	}

	obj := glib.Take(unsafe.Pointer(glg))

	return &GlgLineGraph{Bin: gtk.Bin{Container: gtk.Container{Widget: gtk.Widget{InitiallyUnowned: glib.InitiallyUnowned{Object: obj}}}}}, nil
}

func main() {
	app, err := gtk.ApplicationNew("love.youmu.lgcairo", glib.APPLICATION_FLAGS_NONE)
	if err != nil {
		log.Fatal(err)
	}

	app.Connect("activate", onAppActivate)

	source := glib.TimeoutAdd(1000, fnAddSeriesDataValues)

	app.Run(os.Args)

	gbStop = true

	glib.SourceRemove(source)

}

func onAppActivate(app *gtk.Application) {

	var err error

	win, err := gtk.ApplicationWindowNew(app)
	if err != nil {
		log.Fatal(err)
	}

	glg, err = glgLineGraphNew()
	if err != nil {
		log.Fatal(err)
	}

	glg.Connect("point-selected", fnShowPointSelected)

	C.test_glg_line_graph_data_series_add((*C.GlgLineGraph)(unsafe.Pointer(glg.Native())))

	win.Add(glg)

	win.ShowAll()

}

func fnAddSeriesDataValues() bool {

	if gbStop {
		return true
	}

	if glg != nil {
		C.glg_line_graph_data_series_add_value((*C.GlgLineGraph)(unsafe.Pointer(glg.Native())), 0, C.g_random_double_range(5.0, 20.0))
		C.glg_line_graph_data_series_add_value((*C.GlgLineGraph)(unsafe.Pointer(glg.Native())), 1, C.g_random_double_range(20.0, 30.0))
		C.glg_line_graph_data_series_add_value((*C.GlgLineGraph)(unsafe.Pointer(glg.Native())), 2, 82.7)
		C.glg_line_graph_data_series_add_value((*C.GlgLineGraph)(unsafe.Pointer(glg.Native())), 3, C.g_random_double_range(30.0, 82.0))
		C.glg_line_graph_data_series_add_value((*C.GlgLineGraph)(unsafe.Pointer(glg.Native())), 4, C.g_random_double_range(98.0, 100.0))

		C.glg_line_graph_redraw((*C.GlgLineGraph)(unsafe.Pointer(glg.Native())))
	}

	return true
}

func fnShowPointSelected(widget *glib.Object, xValue float64, yValue float64, pointYPos float64, mouseYPos float64) {

	fmt.Printf("fn_show_point_selected: x_value=%1.0f, y_value=%1.1f, y_pos=%1.1f, mouse_y_pos=%1.1f, ranking=%1.1f\n",
		xValue, yValue, pointYPos, mouseYPos, func() float64 {
			if pointYPos < mouseYPos {
				return pointYPos / mouseYPos * 100
			} else {
				return mouseYPos / pointYPos * 100
			}
		}())

}
