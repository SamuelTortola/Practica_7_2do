import serial
import tkinter as tk
from tkinter import Scrollbar, Toplevel, filedialog
import time


# Variables globales
file_list = []
listing_files = False
activa = 0  # Esta es la variable que controlará el estado de actualización
terminal_text_box = None  # Inicializar variable como None

# Función para leer desde UART y procesar los archivos o contenido recibido
def read_from_uart():
    global listing_files, activa, terminal_text_box  # Declarar las variables globales
    while ser.in_waiting > 0:
        line = ser.readline().decode('utf-8')  # No usar strip() para mantener espacios en blanco
        if line:
            if listing_files and activa == 0:  # Si estamos recibiendo la lista de archivos
                file_list.append(line)
                update_file_menu()
            elif terminal_text_box is not None:  # Verificar si la ventana terminal está abierta
                # Estamos recibiendo el contenido del archivo seleccionado
                terminal_text_box.insert(tk.END, line)
                terminal_text_box.see(tk.END)  # Desplazar el scroll al final
    root.after(100, read_from_uart)

# Función para actualizar el menú desplegable con la lista de archivos
def update_file_menu():
    menu = file_menu['menu']
    menu.delete(0, 'end')  # Limpiar el menú
    for index, file_name in enumerate(file_list):
        menu.add_command(label=file_name, command=lambda idx=index: select_file(idx))
    file_var.set("Seleccionar archivo")  # Restablecer el texto del menú desplegable

# Función para abrir una nueva ventana y mostrar el archivo seleccionado
def open_terminal_window():
    global terminal_text_box  # Definir globalmente para usar en 'read_from_uart'
    
    terminal_window = Toplevel(root)  # Crear una nueva ventana secundaria
    terminal_window.title("Terminal Mode - ASCII Art Viewer")

    # Frame para contener el Text y las scrollbars
    terminal_frame = tk.Frame(terminal_window)
    terminal_frame.pack(fill=tk.BOTH, expand=True)

    # Scrollbars
    x_terminal_scroll = Scrollbar(terminal_frame, orient=tk.HORIZONTAL)
    y_terminal_scroll = Scrollbar(terminal_frame, orient=tk.VERTICAL)

    # Caja de texto para mostrar la salida con scrollbars
    terminal_text_box = tk.Text(terminal_frame, wrap='none', font=('Courier', 12),  # Usar fuente monoespaciada
                                xscrollcommand=x_terminal_scroll.set, yscrollcommand=y_terminal_scroll.set)
    terminal_text_box.grid(row=0, column=0, sticky=tk.NSEW)

    # Configurar las scrollbars
    x_terminal_scroll.config(command=terminal_text_box.xview)
    x_terminal_scroll.grid(row=1, column=0, sticky=tk.EW)
    y_terminal_scroll.config(command=terminal_text_box.yview)
    y_terminal_scroll.grid(row=0, column=1, sticky=tk.NS)

    # Hacer que el frame pueda expandirse
    terminal_frame.grid_rowconfigure(0, weight=1)
    terminal_frame.grid_columnconfigure(0, weight=1)

# Función para seleccionar un archivo y enviar el índice al STM32
def select_file(index):
    ser.write(b'C')  # Envía el comando '4' para leer archivo
    time.sleep(0.3)
    ser.write(str(index).encode())  # Envía el índice del archivo seleccionado

    # Abrir una nueva ventana para mostrar el contenido del archivo
    open_terminal_window()

    global activa  # Declarar 'activa' como global para modificarla
    activa = 1  # Cambiar el valor de la variable global

# Funciones para enviar comandos al STM32
def send_list_files():
    global listing_files
    listing_files = True
    file_list.clear()
    ser.write(b'A')  # Envía el comando '1' para listar archivos
    text_box.insert(tk.END, "\nMostrando nombres de archivos...\n")
    text_box.see(tk.END)  # Desplazar el scroll al final

def send_unmount_sd():
    ser.write(b'B')  # Envía el comando '2' para desmontar la SD
    text_box.insert(tk.END, "\nDesmontando SD...\n")
    text_box.see(tk.END)  # Desplazar el scroll al final
    
    
def send_mount_sd():
    ser.write(b'D')  # Envía el comando '2' para desmontar la SD
    text_box.insert(tk.END, "\nMontando SD...\n")
    text_box.see(tk.END)  # Desplazar el scroll al final
    

# Función para enviar un archivo al STM32
def send_file_to_stm32():
    # Abre el explorador de archivos para que el usuario seleccione un archivo .txt
    file_path = filedialog.askopenfilename(
        title="Seleccionar archivo",
        filetypes=(("Text files", "*.txt"), ("All files", "*.*"))
    )

    # Si el usuario selecciona un archivo
    if file_path:
        try:
            # Abre el archivo .txt en modo lectura
            with open(file_path, 'r') as file:
                ser.write(b'E')  # Envía el comando para indicar que se va a enviar un archivo
                time.sleep(0.2)
                
                # Envía el nombre del archivo al STM32
                file_name = file_path.split('/')[-1]  # Extrae el nombre del archivo
                ser.write(file_name.encode() + b'\n')
                time.sleep(0.2)

                # Envía el contenido del archivo línea por línea
                for line in file:
                    ser.write(line.encode())  # Envía la línea codificada como bytes
                    time.sleep(0.05)  # Pausa pequeña para no saturar la UART

                # Enviar señal de fin de archivo
                ser.write(b'EOF\n')
                print(f"Archivo '{file_name}' enviado al STM32.")

        except FileNotFoundError:
            print(f"El archivo {file_path} no existe.")
        except Exception as e:
            print(f"Error enviando archivo: {str(e)}")
    else:
        print("No se seleccionó ningún archivo.")

# Configuración UART
ser = serial.Serial('COM15', 9600, timeout=1)  # Ajusta el puerto COM según sea necesario

# Interfaz gráfica con Tkinter
root = tk.Tk()
root.title("ASCII Art Viewer, Universidad del Valle de Guatemala")
root.configure(background='#f9e79f')
root.iconbitmap("logo-uvg-1.ico")
lbl = tk.Label(root, text="Generador imagenes ASCII", bg='#33ffb8', font=("Inter", 20))
lbl.pack()

# Frame para contener el Text y las scrollbars
text_frame = tk.Frame(root)
text_frame.pack(fill=tk.BOTH, expand=True)

# Scrollbars
x_scroll = Scrollbar(text_frame, orient=tk.HORIZONTAL)
y_scroll = Scrollbar(text_frame, orient=tk.VERTICAL)

# Caja de texto para mostrar la salida con scrollbars
text_box = tk.Text(text_frame, wrap='none', font=('Courier', 12),  # Usar fuente monoespaciada
                   xscrollcommand=x_scroll.set, yscrollcommand=y_scroll.set)
text_box.grid(row=0, column=0, sticky=tk.NSEW)

# Configurar las scrollbars
x_scroll.config(command=text_box.xview)
x_scroll.grid(row=1, column=0, sticky=tk.EW)
y_scroll.config(command=text_box.yview)
y_scroll.grid(row=0, column=1, sticky=tk.NS)

# Hacer que el frame pueda expandirse
text_frame.grid_rowconfigure(0, weight=1)
text_frame.grid_columnconfigure(0, weight=1)

# Menú desplegable para los archivos
file_var = tk.StringVar(root)
file_menu = tk.OptionMenu(root, file_var, "")
file_menu.pack(pady=10)

# Botones para enviar comandos
button_frame = tk.Frame(root)
button_frame.pack(fill=tk.X)

list_files_button = tk.Button(button_frame, text="Lista de archivos", command=send_list_files)
list_files_button.pack(side=tk.LEFT, padx=5, pady=5)

unmount_button = tk.Button(button_frame, text="Desmontar SD", command=send_unmount_sd)
unmount_button.pack(side=tk.LEFT, padx=5, pady=5)

mount_button = tk.Button(button_frame, text="Montar SD", command=send_mount_sd)
mount_button.pack(side=tk.LEFT, padx=5, pady=5)

# Botón para abrir el explorador de archivos y enviar el archivo seleccionado
upload_button = tk.Button(button_frame, text="Subir archivo", command=send_file_to_stm32)
upload_button.pack(side=tk.LEFT, padx=5, pady=5)

# Iniciar la recepción de datos por UART
root.after(100, read_from_uart)
root.mainloop()
