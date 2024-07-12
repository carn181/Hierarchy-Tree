#include "util.h"
#include <bits/stdc++.h>
#include <cairomm/context.h>
#include <cairomm/fontface.h>
#include <cairomm/surface.h>
#include <cairomm/types.h>
#include <cairommconfig.h>
#include <cstddef>
#include <iostream>
#include <string>

#define SHOW_LOG 1

#ifdef SHOW_LOG
#define LOGT(format, ...)			\
  {						\
    print_tabs(depth);				\
    std::printf(format, __VA_ARGS__);		\
  }
#define LOG(format, ...){std::printf(format, __VA_ARGS__);}
#define SEP() {print_char('=', 100); std::cout << '\n';}
#else
#define LOGT(format, ...)
#define LOG(format, ...)
#define SEP()
#endif

// Principles for this tree
// 1) Nodes should not overlap each other
// 2) Parents must be centered around their children
// 3) There should be a minimum vertical separation between nodes
// 4) Vertical spacing between nodes must not be more than necessary

struct Tree {
  // String displayed by Tree Node
  std::string text;
  // Child Nodes of Tree Node
  std::vector<Tree> children;
  // Naive x,y position of nodes by first pass. Overlaps with other nodes
  double x, y;
  // mod is the vertical adjustment by second pass to space out nodes consistently. Idk what I was doing with shift
  double mod = 0.0;
  double shift = 0.0;
  // Dimensions of the String displayed by node in pixels
  double width = 0, height = 0;
  double top_height;

  Tree(std::string txt, std::vector<Tree> chldrn = {})
    : text(txt)
    , children(chldrn){}

  Tree& front(){ return children.front();}

  Tree& back(){ return children.back();}
  
  void calc_text(Cairo::RefPtr<Cairo::Context> cr, double size) {
    Cairo::TextExtents et;
    auto str_list = splitstring(text, '\n');
    cr->set_font_size(size);

    cr->get_text_extents(str_list.front(), et);
    top_height = et.height;

    for (const auto &str : str_list) {
      Cairo::TextExtents e;
      cr->get_text_extents(str, e);
      width = std::max(width, e.width);
      height += e.height;
    }
  }
};

class HierarchyTree {
private:
  Cairo::RefPtr<Cairo::ToyFontFace> font;
  std::string filename;
  double size;
  double xsep, ysep;

public:
  Cairo::RefPtr<Cairo::Context> cr;
  HierarchyTree(std::string file, double font_size) {
    double width = 1920;
    double height = 1080;
    size = font_size;
    xsep = size / 1.0;
    ysep = size / 1.0;
    filename = file;

    auto surface = Cairo::SvgSurface::create(filename, width, height);
    cr = Cairo::Context::create(surface);

    font = Cairo::ToyFontFace::create("", Cairo::FontSlant::FONT_SLANT_NORMAL,
                                      Cairo::FontWeight::FONT_WEIGHT_NORMAL);
    cr->set_font_face(font);

    cr->save();
    return;
  }

  void export_svg() {
    cr->restore();
    cr->show_page(); // writes file

    std::cout << "Wrote SVG file \"" << filename << "\"\n";
  }

  void set_background() {
    cr->set_source_rgb(1, 1, 1);
    cr->paint();   // fill image with the color
    cr->restore(); // color is back to black now
    cr->save();
  }

  void draw_text(double x, double y, std::string s, double height = 0) {
    auto str_list = splitstring(s, '\n');
    cr->set_font_size(size);
    for (size_t i = 0; i < str_list.size(); i++) {
      double fsize = (height == 0) ? size : height / str_list.size();
      cr->move_to(x, y-height+fsize + static_cast<double>(i) * fsize);
      cr->show_text(str_list[i]);
    }
  }

  // Check this func for errors
  void draw_paren(double x, double y, double y1, double y2, double width) {
    cr->set_source_rgba(0.0, 0.0, 0.0, 0.7);
    // Parenthesis
    auto x1 = x + 2 * width / 3;
    //    std::cout << y1 << " " << y << " " << y2 << '\n';

    // Upper Curve
    auto height = y - y1;
    cr->move_to(x, y);
    cr->line_to(x + width / 3, y);
    cr->curve_to(x1, y, x1, y - height * 0.3, x1, y - height * 0.3);
    cr->line_to(x1, y1 + height * 0.3);
    cr->curve_to(x1, y1, x1 + width / 3, y1, x1 + width / 3, y1);
    cr->line_to(x + width, y1);

    // Lower Curve
    height = y2 - y;
    cr->move_to(x, y);
    cr->line_to(x + width / 3, y);

    cr->curve_to(x1, y, x1, y + height * 0.3, x1, y + height * 0.3);
    cr->line_to(x1, y2 - height * 0.3);
    cr->curve_to(x1, y2, x1 + width / 3, y2, x1 + width / 3, y2);
    cr->line_to(x + width, y2);

    cr->set_line_join(Cairo::LineJoin::LINE_JOIN_ROUND);

    // Text
    cr->stroke();
  }

  // Prints info of each node
  void print_tree(Tree t, int depth = 0) {
    print_tabs(depth);
    std::cout << t.text << " at " << t.x << "x" << t.y << "+" << t.mod << "\n";

    for (auto &child : t.children) {
      print_tree(child, 1 + depth);
    }
  }

  // Assigns naive x,y coordinates for every node with overlapping
  int first_pass(Tree *t, double x, double y) {
    t->calc_text(cr, size);
    LOG("SETTING %s as %f,%f with width %f and height %f \n", t->text.c_str(), x, y, t->width, t->height);
    auto str_list = splitstring(t->text,'\n');
    t->x = x;
    t->y = y+t->height-(t->height/str_list.size());

    // If not leaf node, assign coordiantes for every child node
    if (!t->children.empty()) {
      double k = 0;
      for (auto &child : t->children) {
        first_pass(&child, t->x + t->width + xsep + 10, y + k);
        k += child.height;
      }
    }
    
    // Centering parent nodes
    // if (!t->children.empty() && t->children.size() != 1) {
    //     t->y = ((t->back().y +
    //     	 (t->front().y - t->front().height)) / 2.0 +
    //     	(t->height/2.0));
    //     std::printf("CENTERING %s as %f between %s(%f) & %s(%f)\n",
    //     	    t->text.c_str(), t->y
    //     	    , t->back().text.c_str(), t->back().y
    //     	    , t->front().text.c_str(), t->front().y);
    // }
    return 0;
  }

  // Shifts y coordinates down for overlapping nodes
  // TODO: Need to equal almost equal floats (499.9999 and 500.0)
  int second_pass(Tree *t, int depth = 0) {
    LOGT("%s: STARTING PASS\n", t->text.c_str());

    // If Leaf Node then no children nodes to fix
    if (t->children.size() == 0) {
      LOGT("%s: LEAF NO PASS\n", t->text.c_str());
      return 0;
    }

    for (auto &child : t->children) {
      second_pass(&child, depth + 1);
    }

    LOGT("%s: Starting possible separation\n", t->text.c_str());

    // Checking every pair of children nodes
    for (size_t i = 0, j = 1;
         i <= t->children.size() - 1 && j <= t->children.size() - 1; i++, j++) {
      double xl, xr, yl, yr;
      Tree *l_contour = &t->children.at(i);
      Tree *r_contour = &t->children.at(j);

      do {
        LOGT("%s<%s,%s>: LOOP START MOD: %f\n", t->text.c_str(),
             l_contour->text.c_str(), r_contour->text.c_str(), l_contour->mod);
        yl = l_contour->y;
        yr = r_contour->y - r_contour->height;

        xr = r_contour->x + r_contour->width;
        xl = l_contour->x + l_contour->width;

        
        LOGT("%s<%s,%s>: %f, %f+%f vs %f, %f+%f\n", t->text.c_str(),
             l_contour->text.c_str(), r_contour->text.c_str(), xl, yl, l_contour->mod, xr, yr, r_contour->mod);

        // WHERE THE MAIN SHIFTING MAGIC HAPPENS
        if (yl + l_contour->mod > yr + r_contour->mod) {
          SEP()
          LOGT("MOVE %s BY %f\n", t->children.at(j).text.c_str(), yl - yr + l_contour->mod + r_contour->mod);
          t->children.at(j).mod = std::max(t->children.at(j).mod,
                                           (yl + l_contour->mod) - (yr + r_contour->mod));
          SEP();
        }

        if (xl <= xr) {
          if (l_contour->children.empty())
            break;
          LOGT("CHANGING L_CONTOUR FROM %s, size = %lu to %s, size = %lu\n",
               l_contour->text.c_str(), l_contour->children.size(),
               l_contour->front().text.c_str(),
               l_contour->back().children.size());
          l_contour = &l_contour->back();
          LOGT("CHANGED L_CONTOUR TO %s\n", l_contour->text.c_str());
        }
        if (xl >= xr) {
          if (r_contour->children.empty())
            break;
          LOGT("CHANGING R_CONTOUR FROM %s, size = %lu to %s, size = %lu\n",
               r_contour->text.c_str(), r_contour->children.size(),
               r_contour->front().text.c_str(),
               r_contour->front().children.size());
          r_contour = &r_contour->front();
          LOGT("CHANGED R_CONTOUR TO %s\n", r_contour->text.c_str());
        }

        LOGT("%s<%s,%s>: LOOP DONE\n", t->text.c_str(), l_contour->text.c_str(),
             r_contour->text.c_str());

      } while (1);
    }
    if(t->children.size()>1)
      t->shift = ((t->height/2.0)+((t->front().y + t->back().y)/2.0)) - t->y;
    LOGT("%s: PASS DONE\n", t->text.c_str());
    return 0;
  }

  int draw_subtree(Tree t, double mod = 0.0) {
    draw_text(t.x, t.y + mod + t.mod, t.text, t.height);
    for (auto &child : t.children) {
      draw_subtree(child, mod + t.mod);
    }

    if (!t.children.empty()) {
      double x = t.x + t.width;
      double y = t.y - (t.height / 2.0) + mod + t.mod;
      double y1 = t.front().y + mod + t.mod + t.front().mod -
	t.front().top_height;
      double y2 = t.back().y + mod + t.mod + t.back().mod;
      double width = xsep;
      draw_paren(x+5, y, y1, y2, width);
      //      draw_paren(t.x+t.width, t.y-t.height/2+mod+t.mod,
      //      t.children.front().y+mod+t.mod-t.children.front().height,
      //  	t.children.back().y+mod+t.mod,xsep(size));
    }
    return 0;
  }

  int draw_diagram(Tree t, double x, double y) {
    first_pass(&t, x, y);
    second_pass(&t);
    draw_subtree(t);
    print_tree(t);
    cr->move_to(0, 300.000000);
    cr->line_to(1920, 300.000000);
    cr->stroke();
    return 0;
  }
};
