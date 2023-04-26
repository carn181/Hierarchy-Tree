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
    void draw_text(int x, int y, int size, std::string s){
      auto font = Cairo::ToyFontFace::create("Amiko", Cairo::FontSlant::FONT_SLANT_NORMAL, Cairo::FontWeight::FONT_WEIGHT_NORMAL);
      cr->move_to(x, y);
      cr->set_font_face(font);
      cr->set_font_size(size);
      cr->show_text(s);
    }

    void draw_paren(int x, int y, double width, double height){
        cr->move_to(x - width, y);
        cr->curve_to(x + width, y, 
                     x + width, y + height, 
                     x + 3 * width, y + height);
        cr->move_to(x - width, y);
        cr->curve_to(x + width, y, 
                     x + width, y - height, 
                     x + 3 * width, y - height); 
        draw_text(x + 3 * width, y - height + height/3, height/3, "Testing bruh");
        draw_text(x + 3 * width, y - height + 2*height/3, height/3, "Testing bruh");
        draw_text(x + 3 * width, y - height + 3*height/3, height/3, "Testing bruh");
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
    cr->set_source_rgb(1, 1, 1);
    cr->paint();    // fill image with the color
    cr->restore();  // color is back to black now
     cr->save();
    
    cr->set_source_rgba(0.0, 0.0, 0.0, 0.7);
    t.draw_paren(100, 540, 30, 100);

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
