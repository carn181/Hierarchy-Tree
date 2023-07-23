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

struct Tree {
  std::string text;
  std::vector<Tree> children;
  double x, y;
  double mod = 0.0;
  double width, height;
  double top_height;

  Tree(std::string txt, std::vector<Tree> chldrn = {}) {
    text = txt;
    children = chldrn;
  }

  void calc_text(Cairo::RefPtr<Cairo::Context> cr, double size) {
    height = 0; // JUST TO BE SURE
    Cairo::TextExtents extents;
    auto str_list = splitstring(text, '\n');
    cr->set_font_size(size);

    cr->get_text_extents(str_list.front(), extents);
    top_height = extents.height;

    for (const auto &str : str_list) {
      cr->get_text_extents(str, extents);

      width = std::max(width, extents.width);
      height += extents.height;
    }
    height += 5;
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

  void draw_text(double x, double y, std::string s) {
    auto str_list = splitstring(s, '\n');
    cr->set_font_size(size);
    for (size_t i = 0; i < str_list.size(); i++) {
      cr->move_to(x, y + static_cast<double>(i) * size);
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

  int first_pass(Tree *t, double x, double y, int start = -1) {
    if (start == -1)
      t->calc_text(cr, size);
    t->x = x;
    t->y = y;
    //    draw_text(t->x, t->y, size, t->text);

    if (t->children.size() > 0) {
      double k = 0;
      for (auto &child : t->children) {
        child.calc_text(cr, size);
        first_pass(&child, t->x + t->width + xsep, y + k, 0);
        k += child.height + ysep;
      }
    }
    if (!t->children.empty()) {
      if (t->children.size() != 1) {
        std::printf("Centering %s\n", t->text.c_str());
        t->y = ((t->children.back().y +
                 (t->children.front().y - t->children.front().height)) /
                2.0);
      }
    }
    return 0;
  }

#define LOG(format, ...)                                                       \
  {                                                                            \
    print_tabs(depth);                                                         \
    std::printf(format, __VA_ARGS__);                                          \
  }

  // TODO: Need to equal almost equal floats (499.9999 and 500.0)
  int second_pass(Tree *t, int depth = 0) {
    LOG("%s: STARTING PASS\n", t->text.c_str());

    if (t->children.size() == 0) {
      LOG("%s: LEAF NO PASS\n", t->text.c_str());
      return 0;
    }

    for (auto &child : t->children) {
      second_pass(&child, depth + 1);
    }

    LOG("%s: Starting possible separation\n", t->text.c_str());

    for (size_t i = 0, j = 1;
         i <= t->children.size() - 1 && j <= t->children.size() - 1; i++, j++) {
      double xl, xr, yl, yr;
      Tree *l_contour = &t->children.at(i);
      Tree *r_contour = &t->children.at(j);

      do {
        LOG("%s<%s,%s>: LOOP START MOD: %f\n", t->text.c_str(),
            r_contour->text.c_str(), l_contour->text.c_str(), l_contour->mod);
        yl = l_contour->y;
        yr = r_contour->y - r_contour->height;

        if (yl + l_contour->mod > yr + r_contour->mod) {
          // move by yr-l
          print_char('=', 100);
          std::cout << '\n';
          LOG("MOVE %s BY %f\n", t->children.at(j).text.c_str(),
              yl - yr + l_contour->mod + ysep);
          t->children.at(j).mod = std::max(
              t->children.at(j).mod, (yl + l_contour->mod) - (yr) + ysep + 10);
          print_char('=', 100);
          std::cout << '\n';
        }
        xr = r_contour->x + r_contour->width;
        xl = l_contour->x + l_contour->width;

        LOG("%s<%s,%s>: %f, %f vs %f, %f\n", t->text.c_str(),
            r_contour->text.c_str(), l_contour->text.c_str(), xr, yr, xl, yl);

        if (xl <= xr) {
          if (l_contour->children.empty())
            break;
          LOG("CHANGING L_CONTOUR FROM %s, size = %lu to %s, size = %lu\n",
              l_contour->text.c_str(), l_contour->children.size(),
              l_contour->children.front().text.c_str(),
              l_contour->children.front().children.size());
          l_contour = &l_contour->children.back();
          LOG("CHANGED L_CONTOUR TO %s\n", l_contour->text.c_str());
        }
        if (xl >= xr) {
          if (r_contour->children.empty())
            break;
          LOG("CHANGING R_CONTOUR FROM %s, size = %lu to %s, size = %lu\n",
              r_contour->text.c_str(), r_contour->children.size(),
              r_contour->children.front().text.c_str(),
              r_contour->children.front().children.size());
          r_contour = &r_contour->children.front();
          LOG("CHANGED R_CONTOUR TO %s\n", r_contour->text.c_str());
        }

        LOG("%s<%s,%s>: LOOP DONE\n", t->text.c_str(), r_contour->text.c_str(),
            l_contour->text.c_str());

      } while (1);
    }
    LOG("%s: PASS DONE\n", t->text.c_str());
    return 0;
  }

  int draw_subtree(Tree t, double mod = 0.0) {
    draw_text(t.x, t.y + mod + t.mod, t.text);
    for (auto &child : t.children) {
      draw_subtree(child, mod + t.mod);
    }
    std::cout << "Drawing " << t.text << "= {";
    for (auto &child : t.children) {
      std::cout << child.text << "(" << child.y + mod + child.mod << "),";
    }
    std::cout << "}\n";

    if (!t.children.empty()) {
      double x = t.x + t.width;
      double y = t.y - (t.height / 2.0) + mod + t.mod;
      double y1 = t.children.front().y + mod + t.mod + t.children.front().mod -
                  t.children.front().top_height;
      double y2 = t.children.back().y + mod + t.mod + t.children.back().mod;
      double width = xsep;
      draw_paren(x, y, y1, y2, width);
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
    //    draw_parens(t, size);
    print_tree(t);
    return 0;
  }
};
