// TODO
// - draw_paren with starting and ending y positions [Y]
// - orient draw_tree and draw_text to be based on font_size
// - divide the drawing process into an aesthetically pleasing arrangement of
// child texts and then connecting top child, parent and bottom child with
// parens for all nodes
// - a separate function might be needed for the arranging process and I believe
// the bounds of each text would be required for this
//
// stuff to polish the program once drawing is handled
// - calculating max width and size for the document
// - handle commandline arguments
// - Good readme.md
// - parse trees from a file
// - a better name ig

#include "cairomm/types.h"
#include <algorithm>
#include <bits/stdc++.h>
#include <cairomm/context.h>
#include <cairomm/fontface.h>
#include <cairomm/surface.h>
#include <cairommconfig.h>
#include <cmath>
#include <iostream>
#include <string>

#define TREE_WIDTH 30

std::vector<std::string> splitstring(std::string s, char del) {
  std::vector<std::string> str_list;
  std::stringstream ss(s);
  while (getline(ss, s, del)) {
    str_list.push_back(s);
  }
  return str_list;
}

int count_occurences(std::string s, char c) {
  int count = 0;
  for (auto &chr : s) {
    if (chr == c)
      count++;
  }
  return count;
}

void print_char(char c, int n) {
  for (int i = 1; i <= n; i++)
    std::cout << c;
}

void print_tabs(int n) { print_char('\t', n); }

struct Tree {
  std::string text;
  std::vector<Tree> children;
  double x, y;
  double mod;
  double width, height;
  Tree(std::string txt, std::vector<Tree> chldrn = {}) {
    text = txt;
    children = chldrn;
  }
  void calc_text(Cairo::RefPtr<Cairo::Context> cr, int size) {
    height = 0; // JUST TO BE SURE
    Cairo::TextExtents extents;
    auto str_list = splitstring(text, '\n');
    cr->set_font_size(size);
    for (const auto &str : str_list) {
      cr->get_text_extents(str, extents);

      width = std::max(width, extents.width);

      height -= extents.y_bearing;
    }
  }
};

class HierarchyTree {
private:
  Cairo::RefPtr<Cairo::ToyFontFace> font;
  std::string filename;

public:
  Cairo::RefPtr<Cairo::Context> cr;
  HierarchyTree(std::string file) {
    double width = 1920;
    double height = 1080;
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

  void draw_text(int x, int y, int size, std::string s) {
    auto str_list = splitstring(s, '\n');
    cr->set_font_size(size);
    for (auto i = 0ul; i < str_list.size(); i++) {
      cr->move_to(x, y + i * size);
      cr->show_text(str_list[i]);
    }
  }
  // WILL BE REMOVED SOON, THERE JUST FOR WORKING LOL
  void draw_tree(std::vector<Tree> t, int x, int y, double width,
                 double height) {
    int no_of_newlines = 0;
    for (auto &child : t) {
      no_of_newlines += count_occurences(child.text, '\n');
    }
    std::cout << no_of_newlines << "\n";
    auto lines = no_of_newlines + t.size();
    auto const padding = 5;
    auto rows = lines + padding * (t.size() - 1);
    auto font_size = height / rows;
    // LOOP:
    auto j = 1;

    for (auto i = 0ul; i <= t.size() - 1; i++) {
      draw_text(x + 2 * width, y - height / 2 + font_size * j, font_size,
                t.at(i).text);
      if (t.at(i).children.size() != 0ul) {
        double curr_x, curr_y;
        int longest_letter_width = 30;
        cr->get_current_point(curr_x, curr_y);
        draw_paren(curr_x + longest_letter_width, curr_y - font_size / 2, width,
                   height, t.at(i).children);
      }
      j += padding + 1 + count_occurences(t.at(i).text, '\n');
    }
  }
  // WILL BE REMOVED SOON
  void draw_paren(int x, int y, double width, double height,
                  std::vector<Tree> arr) {
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
    cr->line_to(x1, y1 - width / 3);
    cr->curve_to(x1, y1, x1 + width / 3, y1, x1 + width / 3, y1);
    cr->line_to(x + 2 * width, y1);

    cr->set_line_join(Cairo::LineJoin::LINE_JOIN_ROUND);

    // Text
    draw_tree(arr, x, y, width, height);
    cr->stroke();
  }

  void draw_paren2(int x, int y1, int y2, double width) {
    cr->set_source_rgba(0.0, 0.0, 0.0, 0.7);
    // Parenthesis
    auto x1 = x + 2 * width / 3;
    int y = (y2 + y1) / 2;
    std::cout << y1 << " " << y << " " << y2 << '\n';

    // Upper Curve
    cr->move_to(x, y);
    cr->line_to(x + width / 3, y);
    cr->curve_to(x1, y, x1, y - width / 3, x1, y - width / 3);
    cr->line_to(x1, y1);
    cr->curve_to(x1, y1, x1 + width / 3, y1, x1 + width / 3, y1);
    cr->line_to(x + width, y1);

    // Lower Curve
    cr->move_to(x, y);
    cr->line_to(x + width / 3, y);

    cr->curve_to(x1, y, x1, y + width / 3, x1, y + width / 3);
    cr->line_to(x1, y2 - width / 3);
    cr->curve_to(x1, y2, x1 + width / 3, y2, x1 + width / 3, y2);
    cr->line_to(x + width, y2);

    cr->set_line_join(Cairo::LineJoin::LINE_JOIN_ROUND);

    // Text
    cr->stroke();
  }

  // Prints info of each node
  void print_tree(Tree t, int depth = 0) {
    print_tabs(depth);
    std::cout << t.text << " at " << t.x << "x" << t.y << "\n";

    for (auto &child : t.children) {
      print_tree(child, 1 + depth);
    }
  }

  int first_pass(Tree *t, double x, double y, double size, int start = -1) {
    if (start == -1)
      t->calc_text(cr, size);
    t->x = x + t->width + TREE_WIDTH;
    t->y = y;
    //    draw_text(t->x - t->width, t->y + 500, size, t->text);

    if (t->children.size() > 0) {
      auto k = 0;
      for (auto &child : t->children) {
        child.calc_text(cr, size);
        first_pass(&child, t->x, y + k, size, 0);
        k += child.height;
        if (t->text == "First\nFIRST") {
          std::cout << "height = " << t->height << '\n';
        }
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
      Tree l_contour = t->children.at(i);
      Tree r_contour = t->children.at(j);

      do {
        LOG("%s<%s,%s>: LOOP START\n", t->text.c_str(), r_contour.text.c_str(),
            l_contour.text.c_str());
        yl = l_contour.y;
        yr = r_contour.y - r_contour.height;

        if (yl < yr) {
          // move by yr-l
          print_char('=', 100);
          LOG("MOVE BY %f\n", yr - yl);
          print_char('=', 100);
        }
        xr = r_contour.x;
        xl = l_contour.x;

        LOG("%s<%s,%s>: %f,%f vs %f,%f\n", t->text.c_str(),
            r_contour.text.c_str(), l_contour.text.c_str(), xr, yr, xl, yl);

        if (xl <= xr) {
          if (l_contour.children.empty())
            continue;
          l_contour = l_contour.children.back();
        }
        if (xl >= xr) {
          if (r_contour.children.empty())
            continue;
          r_contour = r_contour.children.front();
        }

        LOG("%s<%s,%s>: LOOP DONE\n", t->text.c_str(), r_contour.text.c_str(),
            l_contour.text.c_str());

      } while (!r_contour.children.empty() && !l_contour.children.empty());
    }
    LOG("%s: PASS DONE\n", t->text.c_str());
    return 0;
  }

  int draw_subtree(Tree t, double size) {
    draw_text(t.x - t.width, t.y, size, t.text);
    for (auto &child : t.children) {
      draw_subtree(child, size);
    }
    return 0;
  }

  int draw_parens(Tree t) {
    if (t.children.size() == 0) {
      return 0;
    } else {
      draw_paren2(t.x + t.width, t.children.at(0).y + t.children.at(0).height,
                  t.children.back().y, TREE_WIDTH);
      for (auto &child : t.children) {
        draw_parens(child);
      }
    }
    return 0;
  }

  int draw_diagram(Tree t, double x, double y, double size) {
    first_pass(&t, x, y, size);
    second_pass(&t);
    draw_subtree(t, size);
    // draw_parens(t);
    print_tree(t);
    return 0;
  }
};

// TESTS

int test_hierarchytree() {
#ifdef CAIRO_HAS_SVG_SURFACE
  HierarchyTree t("hierarch.svg");
  t.set_background();

  auto tr = Tree{"asdsa",
                 {{"First",
                   {{"Second"},
                    {"Second",
                     {{"Third", {{"Fourth"}, {"Fourth"}}},
                      {"Third", {{"Fourth"}, {"Fourth"}}}}}}},
                  {"First"}}};
  auto const goal = Tree{
      "In P.Rami disciplina Philosophica, consider.",
      {{"Idea illius disciplinus, expressa \nin Ciceroniano, qui exemplo "
        "instituti Ciceronis"},
       {"Curriculum opus Philosophicum,partum",
        {{"Exotericum in",
          {{"Grammatica", {{"Latina"}, {"Graeca"}, {"Gallica"}}},
           {"Rhetorica"},
           {"Logicaseu Dialectica"}}},
         {"Acroamaticu in", {{"Arithmetica"}, {"Geometria"}, {"Physica"}}}}}}};

  t.draw_paren(100, 540, 10, 200, goal.children);
  t.export_svg();
  return 0;

#else

  std::cout
      << "You must compile cairo with SVG support for this example to work.\n";
  return 1;

#endif
}

int test_newmodel() {
  HierarchyTree t("new-tree.svg");
  t.set_background();
  auto tr = Tree{
      "Root",
      {{"First",
        {{"Second", {{"Third", {{"Fo"}, {"Fou", {{"Fifth"}}}}}, {"Third"}}},
         {"Second",
          {{"Third", {{"Fourth"}, {"Fourth"}}},
           {"Third", {{"Fourth"}, {"Fourth"}}}}}}},
       {"First"}}};
  t.draw_diagram(tr, 20, 500, 30);
  t.export_svg();
  return 0;
}

int test_drawparen() {
  HierarchyTree t("paren.svg");
  t.set_background();

  t.draw_paren2(400, 100, 500, 20);
  t.draw_text(420, 100 + 20, 20, "Test");
  t.draw_text(420, 500, 20, "Test");
  t.export_svg();

  return 0;
}

int main() { test_newmodel(); }
