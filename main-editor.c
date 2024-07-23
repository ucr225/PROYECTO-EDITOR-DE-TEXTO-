/*
PROYECTO N#1-LAB DE LENGUAJE DE PROGRAMACION
REALIZADO POR URIEL CACERES V-29664183
FECHA DE INICIO 21-11-2023

VERSION 1.1

CUADRO DE TEXTO QUE PERMITA HACER CORRECION ORTOGRAFICA Y DEMAS

ULTIMA EDICION
7/12
4.AM

*/

#include <gtk/gtk.h>           // Incluye la biblioteca GTK
#include <hunspell/hunspell.h> //LIBRERIA DE EDICION DE TEXTO
#include <string.h>

// VARIABLES GLOBALES DE MI PROGRAMA

// Crear un arreglo para almacenar los contadores de caracteres
gint total_len = 0;
GtkWidget *text_view[4];
GtkWidget *frame[4];
GtkTextBuffer *buffer; // bufer del cuadro de texto anterior
GtkEventController *key_controller;
int cuadros = 0; // CONTADOR DE CUADROS
gboolean was_period = FALSE;
// Variable global o estática para el contador de clics
static int n_press = 0; // ES PARA LOS CLICs

// Variable global para el contador de archivos
int totalArchivos = 1;

// PROTOTIPOS DE FUNCIONES

static void Confirmacion(GtkButton *button, gpointer user_data); // ESTA ME MUESTRA UNA VENTANA QUE ME INDICA QUE SE CREO MI ARCHIVO TXT CON EXITO+RUTA DE UBICACION
static void informacion(GtkButton *button, gpointer user_data);  // ESTA FUNCION ME CREA UNA VENTANA CON EL TUTORIAL DE MI PROGRAMA
static void archivo(GtkButton *button, gpointer user_data);
static void mayuscula(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer user_data); // ESTA FUNCION ME PONE EL TEXTO EN MAYUSCULA DESPUES DE UN PUNTO
static void on_insert_text(GtkTextBuffer *buffer, gpointer data);                                               // ESTA FUNCION TIENE TODO LO REFERENTE AL CONTROL DEL CUSOR
gboolean linea_vacia(gpointer data);                                                                            // ESTA VALIDA SI MI LINEA ACTUAL EN DONDE ESTA EL CURSOR PUEDA SUBIR
void buffer_borrar(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end, gpointer data);                 // ACTUALZIA EL BUFFER CADA VEZ QUE BORRO CONTENIDO DE EL
void mover_cursor(GtkTextView *text_view);                                                                      // Esta funcion es la que nos ayuda  a mover el cursor
static void activate(GtkApplication *app, gpointer user_data);                                                  // ESTA FUNCION ES LA QUE ABRE LA VENTANA PRINCIPAL DE MI PROGRAMA
gboolean tabulacion(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);
// BLOQUEO EL TABULADOR PARA EVITAR DESTRUIR MI CUADRO DE TEXTO
void spell_check_and_show(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer user_data); // ESTA FUNCION ES LA QUE HACE EL CORRECTOR ORTOGRAFICO USANDO EL HUNSPELL
void replace_word(GtkWidget *button, gpointer data);
void desbloqueo(GtkButton *button, gpointer user_data);
static void suggestion_clicked_cb(GtkButton *button, gpointer user_data);

void prevent_pegado(GtkTextView *text_view, gpointer user_data);          // ESTA FUNCION BLOQUEA EL PEGAR TEXTO
void on_suggestion_button_clicked(GtkButton *button, gpointer user_data); // PARA CAMBIAR LAS SUGERENCIAS-NO SIRVE,ME CRASHEA TODO QUIZAS ES POR RECURSOS

//  MAIN-AQUI SE EJECUTA TODO
int main(int argc, char **argv)
{
  GtkApplication *app; // Declara un puntero a una aplicaci n
  int status;          // Declara una variable para el estado de salida de la aplicaci n

  app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE); // INICIA MI APP
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);          // CONECTO EL ACTIVADOR DE LA VENTANA CON LA FUNCION CON ESE NOMBRE
  status = g_application_run(G_APPLICATION(app), argc, argv);             // EJECUTO LA APP Y GUARDO EL ESTADO DE SALIDA
  g_object_unref(app);                                                    // LIBERO MEMORIA DE LA APP

  return status; // RETORNO DE ESTATUS
}

// LISTA DE FUNCIONES!!---------------------

// ESTA ME MUESTRA UNA VENTANA QUE ME INDICA QUE SE CREO MI ARCHIVO TXT CON EXITO+RUTA DE UBICACION
static void Confirmacion(GtkButton *button, gpointer user_data)
{
  GtkWidget *box_main, *box_etiquetas; // CAJAS
  GtkWidget *message_label;            // MENSAJE
  GtkWidget *mensaje2;                 // OTRO MENSAJE
  GtkWidget *instrucciones;            // VENTANA
  GtkCssProvider *provider;            // PARA FRONT END

  // CREO LA VENTANA
  instrucciones = gtk_dialog_new();

  // LE PASO EL PROVEEDOR DE CSS PARA PERSONALIZAR
  provider = gtk_css_provider_new();

  // CLASE DE CSS
  gtk_css_provider_load_from_data(provider, ".miClase { background-color: #70e000; font-weight: bold;color: #012a4a;font:condensed 120% sans-serif;font-size:18px }", -1);

  // PASO CSS A MI VENTANA DE DIALOGO
  gtk_widget_add_css_class(instrucciones, "miClase");
  // PASO EL PROVIDER DE CSS AL WIDGET PARA QUE SE APLIQUEN LOS CAMBIOS
  gtk_style_context_add_provider(gtk_widget_get_style_context(instrucciones),
                                 GTK_STYLE_PROVIDER(provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  gtk_window_set_default_size(GTK_WINDOW(instrucciones), 400, 200);          // SIZE DE MI VENTANA
  gtk_window_set_resizable(GTK_WINDOW(instrucciones), FALSE);                // BLOQUEO EL REDIMENCIONAR
  gtk_window_set_icon_name(GTK_WINDOW(instrucciones), "dialog-information"); // ICONO PERO NO SIRVE XD

  gtk_window_set_title(GTK_WINDOW(instrucciones), "ARCHIVO CREADO CON EXITO!! "); // TITULO PARA MI VENTANA

  box_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); // CAJA EN EJE Y

  box_etiquetas = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); // CAJA EN EJE X - CON AMBAS PUEDO CENTRAR TODO

  mensaje2 = gtk_label_new("Estimado usuario... \nSU ARCHIVO HA SIDO CREADO SATISFACTORIAMENTE \n POR FAVOR REVISE EL DIRECTORIO"); // IMPRIMO MI MENSAJE EN LA VENTANA
  gtk_label_set_justify(GTK_LABEL(mensaje2), GTK_JUSTIFY_CENTER);                                                                   // JUSTIFICO AL CENTRO MI TEXTO DE LA ETIQUETA

  gtk_box_append(GTK_BOX(box_etiquetas), mensaje2); // METO LA ETIQUETA EN LA CAJA,ASI SE PUEDE CENTRAR

  message_label = gtk_label_new("\n \n C:\\Users\\uriel\\OneDrive\\Documentos\\LABORATORIO DE LENGUAJE DE PROGRAMACION\\PROYECTO EDITOR DE TEXTO\\tutexto##.txt \n\n NOTA: \n\n EL # Indica el numero de archivo que se creo en base a la cantida de veces que usted presiona el boton de guardar");
  gtk_label_set_justify(GTK_LABEL(message_label), GTK_JUSTIFY_FILL); // LO MISMO ACA

  // Agregar espacio alrededor de la etiqueta
  gtk_widget_set_margin_start(message_label, 15);  // Espacio a la izquierda
  gtk_widget_set_margin_end(message_label, 15);    // Espacio a la derecha
  gtk_widget_set_margin_top(message_label, 15);    // Espacio arriba
  gtk_widget_set_margin_bottom(message_label, 25); // Espacio abajo

  gtk_window_set_title(GTK_WINDOW(instrucciones), "ALERTA"); // TITULO PARA MI VENTANA

  gtk_box_append(GTK_BOX(box_etiquetas), message_label); // METO LA ETIQUETA EN LA CAJA

  gtk_box_append(GTK_BOX(box_main), box_etiquetas);  // RELACIONO LOS WIDGETS UNO DENTRO DE OTROS
  gtk_widget_set_halign(box_main, GTK_ALIGN_CENTER); // ALINEO AL CENTRO HORIZONTAL
  gtk_widget_set_valign(box_main, GTK_ALIGN_CENTER); // IGUAL PERO AL EJE VERTICAL

  gtk_window_set_child(GTK_WINDOW(instrucciones), box_main); // METO UN WIDGET DENTRO DE OTRO

  gtk_widget_show(instrucciones); // APARECE MI VENTANA
}

// ESTA FUNCION ME CREA UNA VENTANA CON EL TUTORIAL DE MI PROGRAMA
static void informacion(GtkButton *button, gpointer user_data)
{
  GtkWidget *box_main, *box_etiquetas; // CAJAS
  GtkWidget *message_label;            // MENSAJE
  GtkWidget *mensaje2;                 // MENSAJE
  GtkWidget *instrucciones;            // VENTANA
  GtkCssProvider *provider;            // PARA EL FRONT END

  // CREO LA VENTANA
  instrucciones = gtk_dialog_new();

  // PARA EL CSS
  provider = gtk_css_provider_new();

  // Definir una clase CSS
  gtk_css_provider_load_from_data(provider, ".miClase { background-color: #F0F3BD; font-weight: bolder ;color: #012a4a;font:condensed 120% sans-serif;font-size:15px }", -1);

  // Aplicar la clase CSS a la ventana de di?logo
  gtk_widget_add_css_class(instrucciones, "miClase");

  gtk_style_context_add_provider(gtk_widget_get_style_context(instrucciones),
                                 GTK_STYLE_PROVIDER(provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  gtk_window_set_default_size(GTK_WINDOW(instrucciones), 500, 250);
  gtk_window_set_resizable(GTK_WINDOW(instrucciones), FALSE);
  gtk_window_set_icon_name(GTK_WINDOW(instrucciones), "dialog-information");

  // TITULO DE MI VENTANA
  gtk_window_set_title(GTK_WINDOW(instrucciones), " INSTRUCCIONES DE USO");

  // Crear una caja vertical principal para organizar los elementos
  box_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  // Crear una caja vertical para las etiquetas
  box_etiquetas = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  // Mensaje directo (etiqueta) en la parte superior
  mensaje2 = gtk_label_new("Estimado usuario...");
  gtk_label_set_justify(GTK_LABEL(mensaje2), GTK_JUSTIFY_CENTER);

  // MENSAJE A LA ETIQUETA
  gtk_box_append(GTK_BOX(box_etiquetas), mensaje2);
  message_label = gtk_label_new("\n\n=====Por favor, lea con atención las siguientes instrucciones:\n\n"
                                "1) El presente editor de texto tiene como objetivo almacenar hasta un total de 4 páginas de 22 líneas cada una. "
                                "Este límite no se puede aumentar.\n\n"
                                "2) Si desea escribir más del límite establecido, le recomendamos presionar el botón 'Guardar' y luego ingresar nuevo texto.\n\n"
                                "3) Al presionar el botón 'Guardar', se estará creando un archivo en formato .txt con el nombre 'tutexto' en su directorio.La ventana emergente le dara la ruta \n "
                                "Este contiene lo escrito por usted. Cada vez que presione el botón, se creará un archivo nuevo.\n\n"
                                "4) El presente editor de texto solo hace correcciones ortográficas en español.\n\n"
                                "5) El texto siempre será justificado.\n\n"
                                "6) Después de escribir un punto (.), la siguiente letra estará en mayúscula de manera automática.\n\n"
                                "7) Los nombres propios no tienen corrección ortográfica.\n\n"
                                "8) La función de  pegar texto está BLOQUEADA, esto es para permitir un correcto desempeño del programa ya que se busca tener un editor de notas rápidas; "
                                "el pegar texto perjudica el buen funcionamiento.\n\n\n"
                                "9) Algunos nombres propios no tienen corrección ortográfica.\n\n"
                                "10) El boton borrar limpia la entrada que esta ingresando en el usuario en los cuadros de texto y te redirecciona al primer cuadro para que inicies la escritura desde cero.\n\n"
                                "=====FAQ (DUDAS FRECUENTES)=====\n\n"
                                "A) Funcionamiento del corrector: La palabra incorrecta va a estar marcada en color rojo para que el usuario vea el listado de sugerencias,el usuario debe presionar entre las sugerencias para asi sustituir la palabra incorrecta\n\n"
                                "B) Cantidad de Archivos .txt: El usuario al presionar GUARDAR creará un .txt por cada vez que presione el botón. "
                                "De existir algún error es recomendable revisar en el directorio si un archivo tiene el mismo nombre, de ser así se recomienda borrarlo/moverlo.\n\n"
                                "C) Si cierro el programa sin haber presionado GUARDAR ¿Pierdo mi texto? Sí, te recomendamos guardar antes de salir para no perder el progreso.\n\n\n"
                                "CONTACTO DEL CREADOR DEL PROGRAMA: caceresuriel22j@gmail.com\n\n\n"
                                "AUTOR: URIEL CACERES V-29664183\n\n\n"
                                "ESTE PROGRAMA FUE CREADO CON FINES ACADÉMICOS ");

  gtk_label_set_justify(GTK_LABEL(message_label), GTK_JUSTIFY_FILL); // JUSTIFICO EL TEXTO

  // Agregar espacio alrededor de la etiqueta
  gtk_widget_set_margin_start(message_label, 15);  // Espacio a la izquierda
  gtk_widget_set_margin_end(message_label, 15);    // Espacio a la derecha
  gtk_widget_set_margin_top(message_label, 15);    // Espacio arriba
  gtk_widget_set_margin_bottom(message_label, 25); // Espacio abajo

  gtk_window_set_title(GTK_WINDOW(instrucciones), "Instrucciones de Uso");

  // A?adir la etiqueta a la caja de etiquetas
  gtk_box_append(GTK_BOX(box_etiquetas), message_label);

  // A?adir la caja de etiquetas a la caja principal
  gtk_box_append(GTK_BOX(box_main), box_etiquetas);
  gtk_widget_set_halign(box_main, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box_main, GTK_ALIGN_CENTER);

  // Establecer la caja principal como hijo de la ventana
  gtk_window_set_child(GTK_WINDOW(instrucciones), box_main);

  // Mostrar la nueva ventana
  gtk_widget_show(instrucciones);
}

// ESTA ME HACE EL ARCHIVO DE TEXTO AL PRESIONAR EL BOTON
static void archivo(GtkButton *button, gpointer user_data)
{
  // Obtiene el puntero a las vistas de texto
  GtkWidget **text_view = (GtkWidget **)user_data;

  // Genera un nombre de archivo TXT
  char informacion[256];
  sprintf(informacion, "C:\\Users\\uriel\\OneDrive\\Documentos\\LABORATORIO DE LENGUAJE DE PROGRAMACION\\PROYECTO EDITOR DE TEXTO\\tutexto.txt"); // DIRECCION DE MI ARCHIVO

  // Abre el archivo para escritura
  FILE *file = fopen(informacion, "w");

  // Itera sobre las cuatro vistas de texto PARA OBTENER LOS BUFFER
  for (int i = 0; i < 4; i++)
  {
    // Obtiene el buffer de la vista de texto actual
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view[i]));

    // Obtiene los iteradores para el inicio y el final del texto en el buffer
    GtkTextIter start_iter, end_iter;
    gtk_text_buffer_get_start_iter(buffer, &start_iter);
    gtk_text_buffer_get_end_iter(buffer, &end_iter);

    // Obtiene el texto del buffer
    gchar *text = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);

    // Escribe el texto en el archivo
    fputs(text, file);

    // Libera la memoria del texto
    g_free(text);
  }

  fclose(file); // CIERRA MI ARCHIVO
}

///
/*
void clear_text_views(GtkWidget *button, gpointer data) {
    // Assuming text_views is your array of GtkTextView objects
    for (int i = 0; i < 4; i++) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_views[i]));
        gtk_text_buffer_set_text(buffer, "", 0);
    }
}
*/
///
static void borrado(GtkButton *button, gpointer user_data)
{
  // LIMPIA LOS CUADROS
  for (int i = 0; i < 4; i++)
  {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view[i]));
    gtk_text_buffer_set_text(buffer, "", 0);
    cuadros = 0;

    gtk_widget_set_visible(text_view[1], FALSE); // HAGO INVISIBLE EL CUADRO
    gtk_widget_set_visible(frame[1], FALSE);     // HAGO INVISIBLE EL MARCO
    gtk_widget_set_visible(text_view[2], FALSE); // HAGO INVISIBLE EL CUADRO
    gtk_widget_set_visible(frame[2], FALSE);     // HAGO INVISIBLE EL MARCO
    gtk_widget_set_visible(text_view[3], FALSE); // HAGO INVISIBLE EL CUADRO
    gtk_widget_set_visible(frame[3], FALSE);     // HAGO INVISIBLE EL MARCO
  }
}

// ESTA FUNCION ME PONE EL TEXTO EN MAYUSCULA DESPUES DE UN PUNTO
static void mayuscula(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer user_data)
{
  if (was_period && g_ascii_islower(text[0])) // SI LA ULTIMA TECLA PRESIONADA ES UN PUNTO Y LA OTRA ES UNA LETRA,MAYUS
  {
    text[0] = g_ascii_toupper(text[0]);
    was_period = FALSE; // REINICIA  PARA PODER SEGUIR USANDO LA FUNCION CON MAS PALABRAS
  }

  if (len == 1 && text[0] == '.')
  {
    was_period = TRUE; // SE PONE TRUE PARA INDICAR EL ULTIMO CARACTER INGRESADO
  }
}

// ESTA FUNCION TIENE TODO LO REFERENTE AL CONTROL DEL CUSOR
static void on_insert_text(GtkTextBuffer *buffer, gpointer data)
{
  GtkTextIter iter;

  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
  gint linea = gtk_text_iter_get_line(&iter) + 1;
  gint columna = gtk_text_iter_get_line_offset(&iter);

  g_print("LINEA DEL CURSOR   Linea %d ------ Columna %d   \n", linea, columna);
  g_print("CUADRO ACTUAL DEL CURSOR : Linea %d   |||||| ", cuadros);

  if (columna == 65)
  {
    gtk_text_buffer_insert(buffer, &iter, "\n", -1);
  }

  ///////
  if (linea == 23 && cuadros < 3)
  {
    cuadros++;
    gtk_widget_set_visible(text_view[cuadros], TRUE);
    gtk_widget_set_visible(frame[cuadros], TRUE);
    gtk_widget_grab_focus(text_view[cuadros]);
  }

  if (linea == 24 && cuadros == 3)
  {
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view[cuadros]), FALSE);
    // gtk_widget_grab_focus(text_view[cuadros]);

    // Crear una ventana emergente con el mensaje "HOLA"
    GtkWidget *emergencia = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(emergencia), "ALERTA!");
    gtk_window_set_default_size(GTK_WINDOW(emergencia), 400, 300);
    gtk_window_set_resizable(GTK_WINDOW(emergencia), FALSE);

    // Crear una etiqueta con el mensaje
    GtkWidget *label = gtk_label_new("ESTIMADO USUARIO \n\n USTED ALCANZO EL LIMITE DE LINEAS DISPONIBLE EN EL PROGRAMA  \n\n SI DESEA ESCRIBIR MAS,PRESIONE GUARDAR PARA GENERAR SU TXT \n\n  BORRE TODO EL CONTENIDO EN LAS CUATRO PAGINAS \n\n ASI PODRA ESCRIBIR NUEVAMENTE  \n USTED SERA REDIRECCIONADO AL PRIMER CUADRO\n SI SE VUELVAN A GENERAR CONSERVARAN LA ANTIGUA ENTRADA\n ESTO ES PARA QUE NO PIERDA LO QUE USTED ESCRIBE POR SI NO GUARDO EN UN ARCHIVO \n\n");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);

    // Crear un botón "OK"
    GtkWidget *ok_button = gtk_button_new_with_label("OK");
    g_signal_connect(ok_button, "clicked", G_CALLBACK(desbloqueo), text_view);
    g_signal_connect_swapped(ok_button, "clicked", G_CALLBACK(gtk_window_destroy), emergencia);

    // Crear un GtkGrid para contener la etiqueta y el botón
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // Añadir la etiqueta al grid
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    // Añadir el botón "OK" al grid
    gtk_grid_attach(GTK_GRID(grid), ok_button, 0, 1, 1, 1);

    // Centrar el botón "OK" horizontalmente y alinear al final verticalmente
    gtk_widget_set_halign(ok_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(ok_button, GTK_ALIGN_END);

    // Establecer el grid como el hijo de la ventana emergente
    gtk_window_set_child(GTK_WINDOW(emergencia), grid);

    // Aplicar CSS para cambiar el color de fondo
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
                                    ".emergencia { background-color: #ef233c; font-weight: bold;color: #22181c ;font:condensed 120% sans-serif;font-size:18px  }", -1);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_widget_add_css_class(emergencia, "emergencia");

    // Mostrar la ventana
    gtk_widget_show(emergencia);

    // VALIDAR QUE SI SE CIERRA LA VENTANA PUES SE PUEDA ESCRIBIR DE NUEVO
  }

  GtkTextIter end_iter;
  gtk_text_buffer_get_end_iter(buffer, &end_iter);
  gint end_line = gtk_text_iter_get_line(&end_iter) + 1;

  // Obtiene el n?mero de l?neas en el cuadro de texto
  gint num_lines = gtk_text_buffer_get_line_count(buffer);
}

// ESTA VALIDA SI MI LINEA ACTUAL EN DONDE ESTA EL CURSOR PUEDA SUBIR
gboolean linea_vacia(gpointer data)
{
  GtkTextView *text_view = GTK_TEXT_VIEW(data);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view); // Tomamos el buffer del cuadro de texto
  GtkTextIter iter;                                            // Iterador para saber la posicion del cursor

  // Tomamos la posicion del cursor
  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
  gint columna = gtk_text_iter_get_line_offset(&iter); // Y aqui tomamos la columna en donde esta el cursor

  // Si la columna esta vacia (El cursor esta en un inicio de linea y termina ahi)
  if (columna == 0 && gtk_text_iter_ends_line(&iter))
  {
    mover_cursor(text_view); // Mueve el cursor arriba al final de la otra linea
  }

  return G_SOURCE_REMOVE; // Removemos la funcion "idle" luego de llamarla una vez
}

// ACTUALZIA EL BUFFER CADA VEZ QUE BORRO CONTENIDO DE EL
void buffer_borrar(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end, gpointer data)
{
  g_idle_add(linea_vacia, data);
}

// Esta funcion es la que nos ayuda  a mover el cursor
void mover_cursor(GtkTextView *text_view)
{
  GtkTextIter iter;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);

  // Tomamos Posicion actual del cursor
  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));

  // Chequeamos si estamos a inicio de linea
  if (gtk_text_iter_starts_line(&iter))
  {
    // Si lo estamos, movemos una posicion
    if (gtk_text_iter_backward_line(&iter))
    {
      // Y lo movemos hacia el final de linea
      gtk_text_iter_forward_to_line_end(&iter);
    }
  }

  // Ponemos el cursor en la nueva posicion
  gtk_text_buffer_place_cursor(buffer, &iter);
}

// BLOQUEO EL TABULADOR PARA EVITAR DESTRUIR MI CUADRO DE TEXTO
gboolean tabulacion(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data)
{
  // Verificar si la tecla presionada es Tab
  if (keyval == GDK_KEY_Tab || keyval == GDK_KEY_ISO_Left_Tab)
  {
    // Bloquear la propagaci?n del evento
    return TRUE;
  }

  // Permitir la propagaci?n del evento para otras teclas
  return FALSE;
}

// ESTA FUNCION ES LA QUE ABRE LA VENTANA PRINCIPAL DE MI PROGRAMA
static void activate(GtkApplication *app, gpointer user_data)
{
  // CREACION DE WIDGETS
  GtkWidget *window; // VENTANA
  GtkWidget *header; // ENCABEZADO DE LA VENTANA
  GtkWidget *button; // BOTONES
  GtkWidget *button2;
  GtkWidget *button3;

  GtkCssProvider *provider;   // PARA MI FRONT END
  GtkWidget *scrolled_window; // VENANA DINAMICA

  GtkTextBuffer *buffer; // BUFFER
  gint limite = 22;      // LIMITE DE LINEAS

  // Creamos una nueva ventana con desplazamiento
  scrolled_window = gtk_scrolled_window_new();
  gtk_window_set_default_size(GTK_WINDOW(scrolled_window), 925, 725); // TAMANO DE LA VENTANA AL CREARSE
  gtk_widget_set_size_request(scrolled_window, 900, 700);             // TAMANO MINIMO AL QUE SE PUEDE MINIMIZAR

  // Creamos una nueva ventana de aplicaciON
  window = gtk_application_window_new(app);
  // Establecemos el t?tulo de la ventana
  gtk_window_set_title(GTK_WINDOW(window), "PULSA AYUDA PARA VER LAS INSTRUCCIONES DE USO");
  // Establecemos el tama?o por defecto de la ventana
  gtk_window_set_default_size(GTK_WINDOW(window), 925, 725);
  gtk_widget_set_size_request(window, 900, 700);

  // Creamos un nuevo encabezado
  header = gtk_header_bar_new();
  gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), TRUE); // VISIBILIDAD DEL HEADER
  gtk_window_set_titlebar(GTK_WINDOW(window), header);                 // POSICIONAR EL HEADER EN LA VENTANA CON JERARQUIA DE OBJETOS

  // Creamos un nuevo bot?n con la etiqueta "AYUDA"
  button = gtk_button_new_with_label("AYUDA");                             // TITULO DEL BOTON
  gtk_widget_add_css_class(button, "button");                              // PASO LA CLASE DEL CSS
  g_signal_connect(button, "clicked", G_CALLBACK(informacion), text_view); // ACCION AL BOTON QUE EJECUTA ESA FUNCION
  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button);               // PASAR UBICACION AL BOTON

  // Creamos un nuevo boton con la etiqueta "SALIR"
  button2 = gtk_button_new_with_label("BORRAR");
  g_signal_connect_swapped(button2, "clicked", G_CALLBACK(borrado), window); // CUANDO CLICKEAS SE CIERRA LA VENTANA

  gtk_widget_add_css_class(button2, "button2"); // lE PASAS EL CSS AL BOTON

  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button2); // LO PONGO EN EL HEADER

  // Creamos un nuevo bot?n con la etiqueta "GUARDAR EN UN .txt" NO TOCAAAARRRRRRRRRRRRRRRRR

  button3 = gtk_button_new_with_label("GUARDAR "); // NOMBRE DEL BOTON
  // A?adimos la clase 'button3' al cuadro de texto
  gtk_widget_add_css_class(button3, "button3");                              // CLASE DE CSS
  g_signal_connect(button3, "clicked", G_CALLBACK(archivo), text_view);      // FUNCION QUE ME CREA EL ARCHIVO
  g_signal_connect(button3, "clicked", G_CALLBACK(Confirmacion), text_view); // FUNCION QUE ME SALTA EL MENSAJE DE CONFIRMACION
  // gtk_widget_add_css_class(button3, "my-button");
  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button3); // LO PONGO EN EL HEADER DE LA VENTANA

  // Creamos una caja para contener los cuadros de texto
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  /////////////

  // Creamos un nuevo proveedor de CSS
  provider = gtk_css_provider_new();

  // Cargamos las reglas CSS-CON ESO PASO TODO LO REFERENTE A PERSONALIZACION
  gtk_css_provider_load_from_data(provider,
                                  //"* {font-size: 15px; font-family:monospace}"
                                  "window { background-color: #00A896; }"
                                  //".frame { background-color: transparent;border: 0px; }"
                                  ".button {  color: #0833a2; font-weight: bold;font family:verdana; font-size:14px;}"
                                  ".button2 {  color:#a4161a ; font-weight: bold;font family:verdana;font-size:14px;}"
                                  ".button3 {  color:green; font-weight: bold;font family:verdana;font-size:17px;}"
                                  //"frame{color:#00A896;border:none}"
                                  // #FDF0D5 'Courier New', monospace
                                  ".text_view{ background-color:#F0F3BD ;border: 9px solid #000000; font-family: monospace ; font-size:18px ; color: #000000;text-decoration: none; }",
                                  -1);
  // ACA LE PASO EL CSS AL WIDGET
  gtk_style_context_add_provider(gtk_widget_get_style_context(box),
                                 GTK_STYLE_PROVIDER(provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_style_context_add_provider(gtk_widget_get_style_context(button),
                                 GTK_STYLE_PROVIDER(provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_style_context_add_provider(gtk_widget_get_style_context(button2),
                                 GTK_STYLE_PROVIDER(provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_style_context_add_provider(gtk_widget_get_style_context(button3),
                                 GTK_STYLE_PROVIDER(provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  // gtk_style_context_add_provider(gtk_widget_get_style_context(new_text_view),
  //  GTK_STYLE_PROVIDER(provider),
  // GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  // Aplicamos las reglas CSS a la ventana
  gtk_style_context_add_provider(gtk_widget_get_style_context(window),
                                 GTK_STYLE_PROVIDER(provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  // CENTRO LA CAJA DE LOS CUADROS DE TEXTO PA QUE SE GENEREN CENTRADOS

  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
  gtk_box_set_spacing(GTK_BOX(box), 10);
  // BUCLE QUE ME GENERA MIS CUADROS DE TEXTO,ES UN ARREGLO

  for (int i = 0; i < 4; i++)
  {
    // Creamos un nuevo cuadro de texto
    text_view[i] = gtk_text_view_new();

    key_controller = gtk_event_controller_key_new(); // LE PASO EL CONTROLADOR DE LAS TECLAS

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view[i])); // CREO UN BUFFER PARA CADA CUADRO

    gtk_text_buffer_set_text(buffer, "", 0); // LO INICIO EN CERO

    g_signal_connect(buffer, "changed", G_CALLBACK(on_insert_text), NULL); // FUNCION DE LAS LINEAS

    g_signal_connect(buffer, "delete-range", G_CALLBACK(buffer_borrar), text_view[i]); // FUNCION PARA BORRAR BUFFER
    g_signal_connect(buffer, "insert-text", G_CALLBACK(spell_check_and_show), NULL);   // FUNCION PARA MI CORRECTOR
    // Conecta las funciones al buffer y al controlador de eventos de teclado

    // g_signal_connect(buffer, "changed", G_CALLBACK(lineaVacia), NULL);
    // g_signal_connect(buffer, "changed", G_CALLBACK(cursorMover), NULL);
    // g_signal_connect(buffer, "changed", G_CALLBACK(borrarBuffer), NULL);

    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(tabulacion), NULL);       // BLOQUEO DEL TABULADOR
    g_signal_connect(text_view[i], "paste-clipboard", G_CALLBACK(prevent_pegado), NULL); // BLOQUEO DE PEGADO DE TEXTO
    g_signal_connect(buffer, "insert-text", G_CALLBACK(mayuscula), NULL);                // FUNCION MAYUSCULA DESPUES DEL PUNTO

    gtk_widget_add_controller(GTK_WIDGET(text_view[i]), key_controller); // CONTROLADOR DE LAS TECLAS

    gtk_text_view_set_justification(GTK_TEXT_VIEW(text_view[i]), GTK_JUSTIFY_FILL); // JUSTIFICAR TEXTO

    gtk_style_context_add_provider(gtk_widget_get_style_context(frame[i]),
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION); // PROVEEDOR DE CSS

    gtk_widget_add_css_class(text_view[i], "text_view"); // CLASE DEL CSS

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view[i]), GTK_WRAP_WORD_CHAR); // PARA LO DE LA LINEA INFINITA

    gtk_widget_set_size_request(text_view[i], 745, 475); // DIMENSIONES EN PX DEL CUADRO DE TEXTO

    // MARCO QUE CONTIENE LOS CUADROS DE TEXTO
    frame[i] = gtk_frame_new(NULL);

    // Establecemos un margen superior de 10 unidades
    gtk_widget_set_margin_top(frame[i], 10);
    // ANadimos el cuadro de texto al marco
    gtk_frame_set_child(GTK_FRAME(frame[i]), text_view[i]);
    gtk_widget_add_css_class(frame[i], "frame"); // Agrega esta linea para el css del frame,no sirvio pero lo deje por si acaso

    gtk_box_append(GTK_BOX(box), frame[i]); // METES EL MARCO  A LA CAJA,POR LO TANTO,ASSI TIENES EL TEXT VIEW DENTRO DE LA CAJA

    GtkStyleContext *style_context = gtk_widget_get_style_context(text_view[i]); // CSS DEL CUADRO DE TEXTO,LE ESTA PASANDO EL ESTILO
    gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    if (i != 0)
    {
      gtk_widget_set_visible(text_view[i], FALSE); // HAGO INVISIBLE EL CUADRO
      gtk_widget_set_visible(frame[i], FALSE);     // HAGO INVISIBLE EL MARCO
    }
  }

  // Agregamos un espacio al final del ?ltimo cuadro de texto
  GtkWidget *sFinal = gtk_label_new("");       // ASI NO SE SALEN PEGADOS
  gtk_widget_set_size_request(sFinal, -1, 15); // Ajusta el '15' al tamaNo de espacio que desees

  // Agregamos el texto de "CREDITOS" como una marca de agua
  GtkWidget *watermark = gtk_label_new("      PROYECTO #1 LAB-LENGUAJE DE PROGRAMACIÓN \n            Prof. JOSÉ RAMÍREZ - DICIEMBRE 2023 V-1.2 \n\n  AUTOR: CACERES RIOS URIEL JOSE CONCEPCION\n                                     V.29664183");
  gtk_widget_add_css_class(watermark, "watermark");
  // Establecer la alineación del label a 0.5 para centrarlo horizontalmente
  gtk_label_set_xalign(GTK_LABEL(watermark), 0.5);

  // Establecer la alineación del label a 0.5 para centrarlo verticalmente
  gtk_label_set_yalign(GTK_LABEL(watermark), 0.5);
  gtk_box_append(GTK_BOX(box), watermark);
  gtk_box_append(GTK_BOX(box), sFinal);

  // A?adimos el cuadro de texto a la ventana con desplazamiento
  // gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);
  // A?adimos la caja a la ventana con desplazamiento
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), box);

  // A?adimos la ventana con desplazamiento a la ventana
  gtk_window_set_child(GTK_WINDOW(window), scrolled_window);

  // Mostramos la ventana
  gtk_widget_show(window);
}

// PARA CAMBIAR LAS SUGERENCIAS-NO SIRVE,ME CRASHEA TODO QUIZAS ES POR RECURSOS
// Esta función se llama cuando se hace clic en un botón de sugerencia.
void on_suggestion_button_clicked(GtkButton *button, gpointer user_data)
{
  // Obtiene el GtkTextBuffer asociado al botón que fue presionado.
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER(g_object_get_data(G_OBJECT(button), "buffer"));
  // Obtiene los iteradores que indican el inicio y el fin de la palabra a reemplazar.
  GtkTextIter *start = g_object_get_data(G_OBJECT(button), "start");
  GtkTextIter *end = g_object_get_data(G_OBJECT(button), "end");

  // Obtiene la sugerencia de corrección del texto del botón.
  const char *suggestion = gtk_button_get_label(button);

  // Elimina la palabra incorrecta del buffer de texto.
  gtk_text_buffer_delete(buffer, start, end);
  // Inserta la sugerencia correcta en el lugar donde estaba la palabra incorrecta.
  gtk_text_buffer_insert(buffer, start, suggestion, -1);

  // Libera la memoria de los iteradores para evitar fugas de memoria.
  gtk_text_iter_free(start);
  gtk_text_iter_free(end);
}

// ESTA FUNCION ES LA QUE HACE EL CORRECTOR ORTOGRAFICO USANDO EL HUNSPELL

void spell_check_and_show(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, gpointer user_data)
{

  if (g_ascii_isspace(text[0]) || g_ascii_ispunct(text[0]) || text[0] == '\n') // VERIFICA SI SE METE UN ESPACIO PARA REVISAR SI LA PALABRA TIENE UN ERROR
  {
    // Obtener el inicio y el final de la palabra
    GtkTextIter start = *location; // INICIO DE LA PALABRA
    GtkTextIter end = *location;   // FINAL
    gtk_text_iter_backward_word_start(&start);
    gtk_text_iter_forward_word_end(&end);
    // Calcular la longitud de la palabra
    gint word_length = gtk_text_iter_get_offset(&end) - gtk_text_iter_get_offset(&start);
    if (word_length > 1)
    {
      // Obtener la palabra
      gchar *word = gtk_text_iter_get_text(&start, &end);

      // Inicializar Hunspell
      Hunhandle *hunspell = Hunspell_create("sugerencias.aff", "palabras.dic"); // ASI SE PASA EL ARCHIVO DE SUGERENCIAS Y  DE PALABRAS

      // REVISAMOS SI TIENE ERRORES LA PALABRA O NO
      if (Hunspell_spell(hunspell, word) == 0)
      {
        // SI LA PALABRA ESTA MAL ESCRITA OBTENEMOS LAS SUGERENCIAS
        char **suggestions;

        int num_suggestions = Hunspell_suggest(hunspell, &suggestions, word);                                 // LAS METEMOS EN UNA VARIABLE ENTERA
        GtkTextTag *tag = gtk_text_buffer_create_tag(buffer, NULL, "underline", PANGO_UNDERLINE_ERROR, NULL); // PARA QUE LA PALABRA SE SUBRAYE
        gtk_text_buffer_apply_tag(buffer, tag, &start, &end);                                                 // SE APLICA

        if (num_suggestions > 0) // SI HAY SUGERENCIAS,CREO LA VENTANA QUE ME IMPRIME LAS SUGERENCIAS
        {
          GtkWidget *message_window = gtk_window_new();
          GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
          GtkCssProvider *provider;
          

          

          gtk_window_set_title(GTK_WINDOW(message_window), "Sugerencias....");
          // Establecemos el tama?o por defecto de la ventana
          gtk_window_set_default_size(GTK_WINDOW(message_window), 300, 100);
          gtk_window_set_resizable(GTK_WINDOW(message_window), FALSE);

          // Creamos un nuevo proveedor de CSS
          provider = gtk_css_provider_new();

          // Definir una clase CSS
          gtk_css_provider_load_from_data(provider, ".miClase { background-color: #F0F3BD; font-weight: bold;color: #012a4a;font:condensed 120% sans-serif;font-size:18px }", -1);

          // Aplicar la clase CSS a la ventana de di?logo
          gtk_widget_add_css_class(message_window, "miClase");

          gtk_style_context_add_provider(gtk_widget_get_style_context(message_window),
                                         GTK_STYLE_PROVIDER(provider),
                                         GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

          for (int i = 0; i < num_suggestions; i++)
          {
            // Crear un nuevo botón con la sugerencia como etiqueta
            GtkWidget *suggestion_button = gtk_button_new_with_label(suggestions[i]);

            // Conectar la señal "clicked" al botón de sugerencia
            // g_signal_connect(suggestion_button, "clicked", G_CALLBACK(on_suggestion_button_clicked), buffer);
            // Primero, conectas la señal "clicked" del botón a la función 'suggestion_clicked_cb'
            g_signal_connect(suggestion_button, "clicked", G_CALLBACK(suggestion_clicked_cb), buffer);
            g_signal_connect_swapped(suggestion_button, "clicked", G_CALLBACK(gtk_window_destroy), message_window); // CUANDO CLICKEAS SE CIERRA LA VENTANA

            // Añadir el botón al box
            gtk_box_append(GTK_BOX(box), suggestion_button);
          }

          gtk_window_set_child(GTK_WINDOW(message_window), box);
          gtk_widget_show(message_window);
        }

        // Liberar las sugerencias
        Hunspell_free_list(hunspell, &suggestions, num_suggestions);
      }
      else
      {
        // La palabra estÃƒÂ¡ escrita correctamente
        printf("PALABRA CORRECTA: %s\n", word);
      }

      // Liberar la palabra y el objeto Hunspell
      g_free(word);
      Hunspell_destroy(hunspell);
    }
  }
}

// ESTA FUNCION BLOQUEA EL PEGAR TEXTO
void prevent_pegado(GtkTextView *text_view, gpointer user_data)
{
  g_signal_stop_emission_by_name(text_view, "paste-clipboard"); // BLOQUEA EL PEGADO DEL PORTA PAPELES
}

// ESTA ES PARA DESBLOQUEAR LA ESCRIURA EN LOS ARCHIVOS DE TEXTO
void desbloqueo(GtkButton *button, gpointer user_data)
{

  GtkTextView **text_view = (GtkTextView **)user_data;

  gtk_text_view_set_editable(text_view[cuadros], TRUE); // DESBLOQUEA LA ESCRITURA PARA EL CUADRO DE TEXTO
  gtk_widget_grab_focus(text_view[0]);

  cuadros = 0;
  gtk_widget_set_visible(text_view[1], FALSE); // HAGO INVISIBLE EL CUADRO
  gtk_widget_set_visible(frame[1], FALSE);     // HAGO INVISIBLE EL MARCO
  gtk_widget_set_visible(text_view[2], FALSE); // HAGO INVISIBLE EL CUADRO
  gtk_widget_set_visible(frame[2], FALSE);     // HAGO INVISIBLE EL MARCO
  gtk_widget_set_visible(text_view[3], FALSE); // HAGO INVISIBLE EL CUADRO
  gtk_widget_set_visible(frame[3], FALSE);     // HAGO INVISIBLE EL MARCO
}

// CAMBIA NOMBRE CHAMO
static void suggestion_clicked_cb(GtkButton *button, gpointer user_data)
{
  // Obtener la sugerencia del botón
  const gchar *suggestion = gtk_button_get_label(button);

  // Obtener el buffer de texto
  GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);

  // Obtener la posición del cursor
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));

  // Mover el cursor hacia atrás hasta el inicio de la última palabra
  if (!gtk_text_iter_starts_word(&iter))
  {
    gtk_text_iter_backward_word_start(&iter);
  }

  // Guardar la posición de inicio de la última palabra
  GtkTextIter start_iter = iter;

  // Mover el cursor hacia adelante hasta el final de la última palabra
  gtk_text_iter_forward_word_end(&iter);

  // Guardar la posición de final de la última palabra
  GtkTextIter end_iter = iter;

  // Eliminar la palabra incorrecta
  gtk_text_buffer_delete(buffer, &start_iter, &end_iter);

  // Insertar la sugerencia en la posición de inicio de la última palabra
  gtk_text_buffer_insert(buffer, &start_iter, suggestion, -1);

  // Mover el cursor al final de la palabra insertada
  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
  gtk_text_iter_forward_chars(&iter, g_utf8_strlen(suggestion, -1));
  gtk_text_buffer_place_cursor(buffer, &iter);
}

//////////////
/*
        for (int i = 0; i < num_suggestions; i++) // BUCLE QUE LAS METE
        {
          GtkWidget *message_label = gtk_label_new(suggestions[i]);

          // Conectar la seÃƒÂ±al "clicked" a la funciÃƒÂ³n on_suggestion_clicked

          gtk_box_append(GTK_BOX(box), message_label);
        }

        gtk_window_set_child(GTK_WINDOW(message_window), box);
        gtk_widget_show(message_window);
      }
*/
/////////////////