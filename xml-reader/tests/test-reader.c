#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <xml-reader/xml-reader.h>

static const gchar *xml_test =
"<book-info>\n"
"  <author>Doe, John</author>\n"
"  <title>An XML test</title>\n"
"</book-info>";

static void
test_walk (void)
{
  XmlReader *reader = xml_reader_new ();

  g_assert (xml_reader_load_from_data (reader, xml_test, NULL) != FALSE);

  g_assert (xml_reader_read_start_element (reader, "book-info") != FALSE);
  g_assert (xml_reader_read_start_element (reader, "author") != FALSE);

  g_assert_cmpstr (xml_reader_get_element_name (reader), ==, "author");
  g_assert_cmpstr (xml_reader_get_element_value (reader), ==, "Doe, John");

  xml_reader_read_end_element (reader);

  g_assert_cmpstr (xml_reader_get_element_name (reader), ==, "book-info");

  g_assert (xml_reader_read_start_element (reader, "title") != FALSE);
  g_assert_cmpstr (xml_reader_get_element_name (reader), ==, "title");
  g_assert_cmpstr (xml_reader_get_element_value (reader), ==, "An XML Test");

  xml_reader_read_end_element (reader);
  g_assert_cmpstr (xml_reader_get_element_name (reader), ==, "book-info");

  xml_reader_read_end_element (reader);

  g_object_unref (reader);
}

int
main (int   argc,
      char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/xml-reader/walk", test_walk);

  return g_test_run ();
}
