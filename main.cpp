#include "cairomm/types.h"
#include <cairomm/context.h>
#include <cairomm/fontface.h>
#include <cairomm/surface.h>
#include <cairommconfig.h>
#include <iostream>
#include <string>
#include <cmath>
#include <bits/stdc++.h>

std::vector<std::string> splitstring(std::string s, char del){
  std::vector<std::string> str_list;
  std::stringstream ss(s);
  while (getline(ss, s, del)){
    str_list.push_back(s);
  }
  return str_list;
}

int count_occurences(std::string s, char c){
  int count = 0;
  for(auto i = 0ul; i<=s.size();i++){
    if(s[i] == c) count++;
  }
  return count;
}

class HierarchyTree {
private:
  Cairo::RefPtr<Cairo::Context> cr;
  Cairo::RefPtr<Cairo::ToyFontFace> font;
  std::string filename = "hierarch.svg";
  
public:
  HierarchyTree() {
    double width = 1920;
    double height = 1080;
    
    auto surface = Cairo::SvgSurface::create(filename, width, height);
    cr = Cairo::Context::create(surface);
    
    font = Cairo::ToyFontFace::create("", Cairo::FontSlant::FONT_SLANT_NORMAL, Cairo::FontWeight::FONT_WEIGHT_NORMAL);
    
    cr->save();
    return;
  }

  void export_svg() {
    cr->restore();
    cr->show_page(); // writes file

    std::cout << "Wrote SVG file \"" << filename << "\"" << std::endl;
  }

  void set_background() {
    cr->set_source_rgb(1, 1, 1);
    cr->paint();   // fill image with the color
    cr->restore(); // color is back to black now
    cr->save();
  }

  void draw_text(int x, int y, int size, std::string s) {
    auto str_list = splitstring(s, '\n');
    cr->set_font_face(font);
    cr->set_font_size(size);
    for(auto i = 0ul ; i < str_list.size() ; i++){
      cr->move_to(x,y+i*size);
      cr->show_text(str_list[i]);
    }
  }

  void draw_paren(int x, int y, double width, double height, std::vector<std::string> arr) {
    cr->set_source_rgba(0.0, 0.0, 0.0, 0.7);
    // Parenthesis
    // Upper Curve
    auto x1 = x + 2 * width / 3;
    auto y1 = y - height / 2;

    cr->move_to(x, y);
    cr->line_to(x + width / 3, y);
    cr->curve_to(x1, y, x1, y - width / 3, x1, y - width / 3);
    cr->line_to(x1, y1 + width / 3);
    cr->curve_to(x1, y1, x1 + width / 3, y1, x1 + width / 3, y1);
    cr->line_to(x + 2 * width, y1);

    // Lower Curve
    y1 = y + height / 2;

    cr->move_to(x, y);
    cr->line_to(x + width / 3, y);

    cr->curve_to(x1, y, x1, y + width / 3, x1, y + width / 3);
    cr->line_to(x1, y1-width/3);
    cr->curve_to(x1, y1, x1 + width / 3, y1,
                 x1 + width / 3, y1);
    cr->line_to(x + 2 * width, y1);

    cr->set_line_join(Cairo::LineJoin::LINE_JOIN_ROUND);
   
    // Text
    int no_of_newlines = 0;
    for(auto i = 0ul; i<=arr.size()-1;i++){
      no_of_newlines += count_occurences(arr.at(i),'\n');
    }
    std::cout<<no_of_newlines<<std::endl;
    auto lines = no_of_newlines + arr.size();
    auto const padding = 2;
    auto rows = lines + padding*(arr.size()-1);
    auto font_size = height/rows;
    // LOOP:
    auto j = 1;

    for(auto i = 0ul;i<=arr.size()-1 ; i++){
      draw_text(x+2*width, y - height/2 + font_size*j, font_size, arr.at(i));
      j+=padding+1+count_occurences(arr.at(i), '\n');
    }
    
    cr->stroke();
  }
};


int main() {

#ifdef CAIRO_HAS_SVG_SURFACE
  HierarchyTree t;
  t.set_background();
  std::vector<std::string> v = {"First\nSecond", "Third", "Fourth\nFifth\nlol"};

  t.draw_paren(200, 540, 5, 100,v);

  t.draw_paren(100, 540, 10, 200,v);
  t.export_svg();
  return 0;

#else

  std::cout
      << "You must compile cairo with SVG support for this example to work."
      << std::endl;
  return 1;

#endif
}
