#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <cairomm/context.h>
#include <cairomm/fontface.h>
#include <cairomm/surface.h>
#include <cairommconfig.h>
#include <cairomm/types.h>
#include "hierarch.cpp"


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
auto const goal_2 =
  Tree{"Justifying Faith",
       {{"Definition",
	 {{"\"Certaine Knowledge\""},
	  {"\"Sure Persuasion\""},
	  {"\"Of the free favour of God\""},
	  {"\"In Jesus Christ\""},
          {"\"Sealed in our heartes by the holy Ghost\""}
	  }},
	{"Fruits of Faith",
	 {{"Inward Godliness",
	   {{"Remission of sins"},
	    {"Love of the Father"},
	    {"Promise that prayers will be heard"}}},
	  {"Outward Godliness",
	   {{"Obedience",
	     {{"Obey", {{"Reasons"}, {"Laws"}}},
	      {"Fulfill", {{"Doctrine"}, {"Uses"}}}
	     }
	     },
	    {"Ends",
	     {{"For God's Glory"},
	      {"For our own comfort"},
	      {"For the good of our brethren"}}}}}}}}};
auto const fh_test =
  Tree{"Root",
       {{"A Really Long Line"},
        {"Filled with random words"},
        {"That are impressed on the mind"},
        {"By the faculty of creative imagination"},
        {"To stir up the other mental faculties"},
        {"Like the will, intellect and memory"},
        {"And avoid a deafening numbness"},
        {"Which would be the death of us all"},
        {"This is a bold line\nwith a line break"},
        {"I Do Not like such lines because of the break in reading"}
       }};

void test_model(std::string fn, Tree tr){
  HierarchyTree t(fn,20);
  t.set_background();

  t.draw_diagram(tr, 20, 300);
  t.export_svg();  
}

int test_newmodel() {
  test_model("simp-tree-1.svg", tr);
  test_model("simp-tree-2.svg", simple_tr);
  test_model("new-tree-1.svg", goal);
  test_model("new-tree-2.svg", goal_2);
  test_model("new-tree-3.svg", fh_test);
  return 0;
}

int test_drawparen() {
  HierarchyTree t("paren.svg",20);
  t.set_background();

  t.draw_paren(400, (100+500.0)/2, 100, 500, 20);
  t.draw_text(420, 100 + 20, "Test");
  t.draw_text(420, 500, "Test");
  t.export_svg();

  return 0;
}
