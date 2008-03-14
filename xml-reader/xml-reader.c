/* xml-reader.h: Cursor based XML reader API
 *
 * Copyright (C) 2008  Emmanuele Bassi  <ebassi@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * Author:
 *   Emmanuele Bassi  <ebassi@gnome.org>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include <libxml/entities.h>
#include <libxml/globals.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include <glib.h>

#include "xml-reader.h"

#define G_UNIMPLEMENTED                         G_STMT_START {  \
        g_warning ("%s: Function not implemented", G_STRLOC);   \
                                                } G_STMT_END

#define XML_TO_CHAR(s)  ((char *) (s))

G_DEFINE_TYPE (XmlReader, xml_reader, G_TYPE_OBJECT);

struct _XmlReaderPrivate
{
  guint is_filename : 1;
  gchar *filename;

  guint error_state : 1;
  XmlReaderError last_error;

  gint depth;

  xmlDocPtr current_doc;

  xmlNodePtr parent;
  xmlNodePtr cursor;

  xmlChar *cursor_value;
};

static inline void
xml_reader_clear (XmlReader *reader)
{
  XmlReaderPrivate *priv = reader->priv;

  if (priv->cursor_value)
    {
      xmlFree (priv->cursor_value);
      priv->cursor_value = NULL;
    }

  if (priv->current_doc)
    xmlFreeDoc (priv->current_doc);
}

static void
xml_reader_finalize (GObject *gobject)
{
  XmlReaderPrivate *priv = XML_READER (gobject)->priv;

  g_free (priv->filename);

  xml_reader_clear (XML_READER (gobject));

  G_OBJECT_CLASS (xml_reader_parent_class)->finalize (gobject);
}

static void
xml_reader_class_init (XmlReaderClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (XmlReaderPrivate));

  gobject_class->finalize = xml_reader_finalize;
}

static void
xml_reader_init (XmlReader *reader)
{
  XmlReaderPrivate *priv;

  reader->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE (reader, XML_TYPE_READER, XmlReaderPrivate);

  priv->error_state = FALSE;
  priv->last_error = 0;

  priv->is_filename = FALSE;
  priv->filename = NULL;

  priv->cursor = NULL;
  priv->cursor_value = NULL;
}

/*
 * Public API
 */

/**
 * xml_reader_new:
 *
 * Creates a new #XmlReader instance. You can use the #XmlReader
 * instance to load an XML data stream from a file or a memory
 * buffer and then iterate over it to extract the data contained
 * inside it.
 *
 * Return value: the newly created #XmlReader instance. Use
 *   g_object_unref() when done using it.
 */
XmlReader *
xml_reader_new (void)
{
  return g_object_new (XML_TYPE_READER, NULL);
}

/**
 * xml_reader_load_from_data:
 * @reader: a #XmlReader
 * @buffer: a %NULL terminated string containing an XML stream
 * @error: return location for a #GError, or %NULL
 *
 * Loads the XML into the @reader and sets the initial status of the
 * DOM to be ready to be walked using the #XmlReader methods.
 *
 * If @reader was already being used, the previous state is discarded;
 * this is also true for an #XmlReader instance in an error state.
 *
 * In case of error, %FALSE is returned and @error is set.
 *
 * Return value: %TRUE if the XML data was successfully loaded.
 */
gboolean
xml_reader_load_from_data (XmlReader    *reader,
                           const gchar  *buffer,
                           GError      **error)
{
  XmlReaderPrivate *priv;

  g_return_val_if_fail (XML_IS_READER (reader), FALSE);
  g_return_val_if_fail (buffer != NULL, FALSE);

  priv = reader->priv;

  xml_reader_clear (reader);

  LIBXML_TEST_VERSION;

  priv->current_doc = xmlReadMemory (buffer, strlen (buffer),
                                     NULL, NULL,
                                     XML_PARSE_RECOVER | XML_PARSE_NOBLANKS | XML_PARSE_COMPACT);
  if (!priv->current_doc)
    {
      gchar *error_message;

      if (!priv->is_filename)
        error_message = g_strdup ("Unable to parse XML buffer");
      else
        error_message = g_strdup_printf ("Unable to parse file `%s'",
                                         priv->filename);

      g_set_error (error, XML_READER_ERROR,
                   XML_READER_ERROR_INVALID,
                   error_message);

      g_free (error_message);

      return FALSE;
    }

  priv->parent = priv->current_doc->xmlRootNode;
  priv->cursor = NULL;
  priv->depth = 0;

  return TRUE;
}

/**
 * xml_reader_load_from_file:
 * @reader: a #XmlReader
 * @filename: the full path to an XML file
 * @error: return location for a #GError, or %NULL
 *
 * Loads an XML file at @filename into @reader and sets the #XmlReader
 * to be ready to walk the XML document object model.
 *
 * See also xml_reader_load_from_file().
 *
 * Return value: %TRUE if the file was successfully loaded.
 */
gboolean
xml_reader_load_from_file (XmlReader    *reader,
                           const gchar  *filename,
                           GError      **error)
{
  XmlReaderPrivate *priv;
  GError *internal_error;
  gchar *buffer;
  gboolean retval;

  g_return_val_if_fail (XML_IS_READER (reader), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);

  priv = reader->priv;

  internal_error = NULL;
  if (!g_file_get_contents (filename, &buffer, NULL, &internal_error))
    {
      g_propagate_error (error, internal_error);
      return FALSE;
    }

  g_free (priv->filename);

  priv->is_filename = TRUE;
  priv->filename = g_strdup (filename);

  retval = xml_reader_load_from_data (reader, buffer, &internal_error);
  if (!retval)
    g_propagate_error (error, internal_error);

  g_free (buffer);

  return retval;
}

/**
 * xml_reader_get_error:
 * @reader: a #XmlReader
 * @error: return location for a #GError, or %NULL
 *
 * Checks the state of @reader and returns whether the #XmlReader instance
 * is in an error state and cannot be used to walk the DOM tree or not.
 *
 * #XmlReader API does not require a #GError argument for the DOM tree
 * walking; in case of error, #XmlReader methods will return an invalid
 * value (either %FALSE, %NULL or a negative integer). To check whether an
 * instance is in an error state, xml_reader_get_error() can be used and
 * if it returns %TRUE, the passed #GError can be used to retrieve the
 * last error occurred.
 *
 * Return value: %TRUE is the #XmlReader is in error state
 */
gboolean
xml_reader_get_error (XmlReader  *reader,
                      GError    **error)
{
  g_return_val_if_fail (XML_IS_READER (reader), FALSE);

  if (error && *error == NULL)
    g_set_error (error, XML_READER_ERROR,
                 reader->priv->last_error,
                 "XmlReader error");

  return reader->priv->error_state;
}

/**
 * xml_reader_read_start_element:
 * @reader: a #XmlReader
 * @element_name: the name of the element to position the cursor on
 *
 * Moves the internal cursor to the first element named @element_name
 * inside the XML document object model.
 *
 * For instance, this example will find the &lt;book&gt; element and
 * iterate over its children "author" and "copyright" and retrieve
 * their value:
 *
 * |[
 *   gchar *author, *title;
 *
 *   xml_reader_read_start_element (reader, "book");
 *   xml_reader_read_start_element (reader, "author");
 *   author = g_strdup (xml_reader_get_element_value (reader));
 *   xml_reader_read_end_element (reader);
 *   xml_reader_read_start_element (reader, "title");
 *   title = g_strdup (xml_reader_get_element_value (reader));
 *   xml_reader_read_end_element (reader);
 *   xml_reader_read_end_element (reader);
 *
 *   g_print ("The author of the book '%s' is %s\n", author, title);
 *   g_free (author);
 *   g_free (title);
 * ]|
 *
 * Return value: %TRUE if the cursor positioning was successful
 */
gboolean
xml_reader_read_start_element (XmlReader   *reader,
                               const gchar *element_name)
{
  XmlReaderPrivate *priv;
  xmlNodePtr cursor, node;

  g_return_val_if_fail (XML_IS_READER (reader), FALSE);
  g_return_val_if_fail (element_name != NULL, FALSE);

  if (xml_reader_get_error (reader, NULL))
    return FALSE;

  priv = reader->priv;

  cursor = priv->cursor;
  if (!cursor)
    cursor = priv->current_doc->xmlRootNode;
  else
    cursor = cursor->xmlChildrenNode;

  for (node = cursor;
       node != NULL;
       node = node->next)
    {
      if (node->type == XML_ELEMENT_NODE &&
          node->name != NULL &&
          strcmp (XML_TO_CHAR (node->name), element_name) == 0)
        {
          xmlNodePtr child;

          priv->parent = priv->cursor;
          priv->cursor = node;
          priv->depth += 1;

          /* preload the text, if any */
          child = priv->cursor->xmlChildrenNode;
          if (child && xmlNodeIsText (child))
            {
              if (priv->cursor_value)
                xmlFree (priv->cursor_value);

              priv->cursor_value = xmlNodeGetContent (child);
            }

          return TRUE;
        }
    }

  priv->error_state = TRUE;
  priv->last_error = XML_READER_ERROR_UNKNOWN_NODE;

  return FALSE;
}

/**
 * xml_reader_read_end_element:
 * @reader: a #XmlReader
 *
 * Positions the internal cursor at the end of the current element
 */
void
xml_reader_read_end_element (XmlReader *reader)
{
  XmlReaderPrivate *priv;

  g_return_if_fail (XML_IS_READER (reader));

  priv = reader->priv;

  if (!priv->cursor)
    return;

  if (priv->cursor_value)
    {
      xmlFree (priv->cursor_value);
      priv->cursor_value = NULL;
    }

  priv->depth -= 1;

  priv->cursor = priv->parent;
  if (!priv->cursor)
    priv->cursor = priv->current_doc->xmlRootNode;
}

/**
 * xml_reader_get_element_name:
 * @reader: a #XmlReader
 *
 * Retrieves the name of the element the cursor is currently on.
 *
 * Return value: the name of the current element. The string is owned
 *   by the #XmlReader instance and should never be modified or freed.
 */
G_CONST_RETURN gchar *
xml_reader_get_element_name (XmlReader *reader)
{
  XmlReaderPrivate *priv;

  g_return_val_if_fail (XML_IS_READER (reader), NULL);

  priv = reader->priv;

  if (priv->cursor)
    return XML_TO_CHAR (priv->cursor->name);

  return NULL;
}

/**
 * xml_reader_get_element_value:
 * @reader: a #XmlReader
 *
 * Retrieves the value of the element the cursor is currently on.
 *
 * Return value: the value of the current element. The string is owned
 *   by the #XmlReader instance and should never be modified or freed.
 */
G_CONST_RETURN gchar *
xml_reader_get_element_value (XmlReader *reader)
{
  XmlReaderPrivate *priv;

  g_return_val_if_fail (XML_IS_READER (reader), NULL);

  priv = reader->priv;

  if (xml_reader_get_error (reader, NULL))
    return NULL;

  if (priv->cursor_value)
    return XML_TO_CHAR (priv->cursor_value);

  return NULL;
}

gboolean
xml_reader_has_attributes (XmlReader *reader)
{
  G_UNIMPLEMENTED;
  return FALSE;
}

gint
xml_reader_count_attributes (XmlReader *reader)
{
  G_UNIMPLEMENTED;
  return FALSE;
}

gboolean
xml_reader_read_attribute_pos (XmlReader *reader,
                               gint       index_)
{
  G_UNIMPLEMENTED;
  return FALSE;
}

gboolean
xml_reader_read_attribute_name (XmlReader   *reader,
                                const gchar *attribute_name)
{
  G_UNIMPLEMENTED;
  return FALSE;
}

G_CONST_RETURN gchar *
xml_reader_get_attribute_value (XmlReader *reader)
{
  G_UNIMPLEMENTED;
  return NULL;
}

GQuark
xml_reader_error_quark (void)
{
  return g_quark_from_static_string ("xml-reader-error-quark");
}
