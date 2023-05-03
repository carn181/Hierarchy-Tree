#include "cairomm/types.h"
#include <string>
#include <iostream>
#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <cairomm/fontface.h>

#include <cmath>

class HierarchyTree{
  private:
    Cairo::RefPtr<Cairo::Context> cr;
  public:
    HierarchyTree(Cairo::RefPtr<Cairo::Context> context){
      cr = context;
    }
    void set_background(){
      cr->set_source_rgb(1, 1, 1);
      cr->paint();    // fill image with the color
      cr->restore();  // color is back to black now
      cr->save();
    }
    void draw_text(int x, int y, int size, std::string s){
      auto font = Cairo::ToyFontFace::create("Amiko", Cairo::FontSlant::FONT_SLANT_NORMAL, Cairo::FontWeight::FONT_WEIGHT_NORMAL); 
     
      cr->move_to(x, y);
      cr->set_font_face(font);
      cr->set_font_size(size);
      cr->show_text(s);
    }

    void draw_paren(int x, int y, double width, double height){
        // Upper Curve
        auto x1 = x + 2*width/3;
        auto y1 = y - height/2;
        
        cr->move_to(x,y);
        cr->line_to(x + width/3, y);
        
        cr->curve_to(x1, y,
                     x1, y - width/6,
                     x1, y - width/6);
        cr->line_to(x1, y1 + width/3);
        cr->curve_to(x1, y1,
                     x1 + width/3,y1,
                     x1 + width/3,y1);
        cr->line_to(x + 2 * width,y1);
        
        // Lower Curve
        x1 = x + 2*width/3;
        y1 = y + height/2;
        
        cr->move_to(x,y);
        cr->line_to(x + width/3, y);
        
        cr->curve_to(x1, y,
                     x1, y + width/6,
                     x1, y + width/6);
        cr->line_to(x1, y1);
        cr->curve_to(x1, y1 + width/3,
                     x1 + width/3,y1 + width/3,
                     x1 + width/3,y1 + width/3);
        cr->line_to(x + 2 * width,y1 + width/3);


        cr->set_line_join(Cairo::LineJoin::LINE_JOIN_ROUND);


        draw_text(x + width, y + height/2, height/3.0, "gesting bruh");
        draw_text(x + width, y + height/2 - height/3, height/3, "Testing bruh");
        draw_text(x + width, y + height/2 - 2*height/3, height/3, "Testing bruh");
        
        cr->stroke();
    }
};



int main()
{

#ifdef CAIRO_HAS_SVG_SURFACE
    std::string filename = "hierarch.svg";
    double width = 1920;
    double height = 1080;
    auto surface =
        Cairo::SvgSurface::create(filename, width, height);

    auto cr = Cairo::Context::create(surface);
    HierarchyTree t(cr);
  
    cr->save(); // save the state of the context
    
    
    t.set_background();

    cr->set_source_rgba(0.0, 0.0, 0.0, 0.7);
    t.draw_paren(100, 540, 50, 100);

    t.draw_paren(400, 540, 50, 400);

    cr->move_to(0, 100);
    cr->line_to(100, 100);
    t.draw_text(0, 100, 30, "Hi");
    cr->stroke();
    
    cr->restore();
    cr->show_page(); // writes file

    std::cout << "Wrote SVG file \"" << filename << "\"" << std::endl;
    return 0;

#else

    std::cout << "You must compile cairo with SVG support for this example to work."
        << std::endl;
    return 1;

#endif
}
