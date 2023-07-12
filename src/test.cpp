#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <cairomm/context.h>
#include <cairomm/fontface.h>
#include <cairomm/surface.h>
#include <cairommconfig.h>
#include <cairomm/types.h>
#include "hierarch.cpp"

int test_newmodel() {
  HierarchyTree t("new-tree.svg",20);
  t.set_background();
  auto tr = Tree{
      "Root",
      {{"First",
        {{"Second", {{"Third", {{"Fo"}, {"Fou", {{"Fifth"}}}}}, {"Third"}}},
         {"Second",
          {{"Third", {{"Fourth"}, {"Fourth"}}},
           {"Third", {{"Fourth"}, {"Fourth"}}}}}}},
       {"First"}}};
  auto simple_tr = Tree{
    "1",
    {{{"2", {{"5"}, {"6"}}},
      {"3", {{"4"}}}}}
  };
    auto const goal = Tree{
      "In P.Rami disciplina Philosophica, consider.",
      {{"Idea illius disciplinus, expressa \nin Ciceroniano, qui exemplo "
        "instituti Ciceronis"},
       {"Curriculum opus Philosophicum,partum",
        {{"Exotericum in",
          {{"Grammatica", {{"Latina"}, {"Graeca"}, {"Gallica"}}},
           {"Logicaseu Dialectica"}}},
         {"Acroamaticu in", {{"Arithmetica"}, {"Geometria"}, {"Physica"}}}}}}};

  t.draw_diagram(tr, 20, 500);
  t.export_svg();
  return 0;
}

int test_drawparen() {
  HierarchyTree t("paren.svg",20);
  t.set_background();

  t.draw_paren(400, 100+500/2, 100, 500, 20);
  t.draw_text(420, 100 + 20, 20, "Test");
  t.draw_text(420, 500, 20, "Test");
  t.export_svg();

  return 0;
}
