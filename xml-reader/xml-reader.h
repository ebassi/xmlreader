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

#ifndef __XML_READER_H__
#define __XML_READER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define XML_TYPE_READER            (xml_reader_get_type ())
#define XML_READER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), XML_TYPE_READER, XmlReader))
#define XML_IS_READER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), XML_TYPE_READER))
#define XML_READER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), XML_TYPE_READER, XmlReaderClass))
#define XML_IS_READER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), XML_TYPE_READER))
#define XML_READER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), XML_TYPE_READER, XmlReaderClass))

/**
 * XML_READER_ERROR:
 *
 * #XmlReader #GError domain.
 */
#define XML_READER_ERROR           (xml_reader_error_quark ())

/**
 * XmlReaderError:
 * @XML_READER_ERROR_INVALID:
 * @XML_READER_ERROR_UNKNOWN_NODE:
 * @XML_READER_ERROR_EMPTY_FILE:
 *
 * #XmlReader error enumeration.
 */
typedef enum {
  XML_READER_ERROR_INVALID,
  XML_READER_ERROR_UNKNOWN_NODE,
  XML_READER_ERROR_EMPTY_FILE
} XmlReaderError;

GQuark xml_reader_error_quark (void);

typedef struct _XmlReader          XmlReader;
typedef struct _XmlReaderPrivate   XmlReaderPrivate;
typedef struct _XmlReaderClass     XmlReaderClass;

/**
 * XmlReader:
 *
 * XML reader object, providing a cursor based API for accessing
 * the data inside an XML stream.
 */
struct _XmlReader
{
  /*< private >*/
  GObject parent_instance;

  XmlReaderPrivate *priv;
};

/**
 * XmlReaderClass:
 *
 * Base class for #XmlReader.
 */
struct _XmlReaderClass
{
  /*< private >*/
  GObjectClass parent_class;
};

GType                 xml_reader_get_type            (void) G_GNUC_CONST;

XmlReader *           xml_reader_new                 (void);
gboolean              xml_reader_load_from_data      (XmlReader    *reader,
                                                      const gchar  *buffer,
                                                      GError      **error);
gboolean              xml_reader_load_from_file      (XmlReader    *reader,
                                                      const gchar  *filename,
                                                      GError      **error);
gboolean              xml_reader_get_error           (XmlReader    *reader,
                                                      GError      **error);

gboolean              xml_reader_read_start_element  (XmlReader    *reader,
                                                      const gchar  *element_name);
void                  xml_reader_read_end_element    (XmlReader    *reader);
G_CONST_RETURN gchar *xml_reader_get_element_name    (XmlReader    *reader);
G_CONST_RETURN gchar *xml_reader_get_element_value   (XmlReader    *reader);

gboolean              xml_reader_has_attributes      (XmlReader    *reader);
gint                  xml_reader_count_attributes    (XmlReader    *reader);
gboolean              xml_reader_read_attribute_pos  (XmlReader    *reader,
                                                      gint          index_);
gboolean              xml_reader_read_attribute_name (XmlReader    *reader,
                                                      const gchar  *attribute_name);
G_CONST_RETURN gchar *xml_reader_get_attribute_value (XmlReader    *reader);

G_END_DECLS

#endif /* __XML_READER_H__ */
