#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <xml-reader/xml-reader.h>

static const gchar *xml_simple_test =
"<?xml version=\"1.0\"?>"
"<book-info>"
  "<author>Doe, John</author>"
  "<title>An XML Test</title>"
"</book-info>";

static const gchar *xml_attr_test =
"<?xml version=\"1.0\"?>"
"<book-info>"
  "<author role=\"primary\">Doe, John</author>"
  "<author role=\"secondary\">Q. John</author>"
"</book-info>";

static void
test_walk (void)
{
  XmlReader *reader = xml_reader_new ();

  g_assert (xml_reader_load_from_data (reader, xml_simple_test, NULL) != FALSE);

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

static void
test_invalid_walk (void)
{
  XmlReader *reader = xml_reader_new ();
  GError *error = NULL;

  g_assert (xml_reader_load_from_data (reader, xml_simple_test, NULL) != FALSE);

  g_assert_cmpint (xml_reader_read_start_element (reader, "book-info"), !=, FALSE);
  g_assert_cmpint (xml_reader_read_start_element (reader, "invalid"), ==, FALSE);
  g_assert_cmpint (xml_reader_get_error (reader, &error), ==, TRUE);
  g_assert_cmpint (error->domain, ==, XML_READER_ERROR);
  g_assert_cmpint (error->code, ==, XML_READER_ERROR_UNKNOWN_NODE);
  g_assert (xml_reader_get_element_name (reader) == NULL);

  xml_reader_read_end_element (reader);

  g_assert_cmpstr (xml_reader_get_element_name (reader), ==, "book-info");

  g_error_free (error);
  g_object_unref (reader);
}

static void
test_attributes (void)
{
  XmlReader *reader = xml_reader_new ();

  g_assert (xml_reader_load_from_data (reader, xml_attr_test, NULL) != FALSE);

  g_assert (xml_reader_read_start_element (reader, "book-info") != FALSE);
  g_assert (xml_reader_read_start_element (reader, "author") != FALSE);

  g_assert_cmpstr (xml_reader_get_element_name (reader), ==, "author");
  g_assert_cmpstr (xml_reader_get_element_value (reader), ==, "Doe, John");

  g_assert_cmpint (xml_reader_has_attributes (reader), ==, TRUE);
  g_assert_cmpint (xml_reader_count_attributes (reader), ==, 1);
  g_assert_cmpint (xml_reader_read_attribute_name (reader, "role"), ==, TRUE);
  g_assert_cmpstr (xml_reader_get_attribute_value (reader), ==, "primary");

  xml_reader_read_end_element (reader);
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
  g_test_add_func ("/xml-reader/invalid", test_invalid_walk);
  g_test_add_func ("/xml-reader/attributes", test_attributes);

  return g_test_run ();
}
