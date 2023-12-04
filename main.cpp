#include <iostream>
#include <mysql.h>
#include <string>
#include <limits>
#include <cstdlib>
#include <thread>
#include <conio.h>

// Función para establecer la conexión con la base de datos
MYSQL* establecerConexion(const char* host, const char* usuario, const char* contrasena, const char* baseDatos) {
	MYSQL* conn = mysql_init(NULL);
	
	if (conn == NULL) {
	    std::cerr << "Error al inicializar la conexión: " << mysql_error(conn) << std::endl;
	    return NULL;
	}
	
	if (mysql_real_connect(conn, host, usuario, contrasena, baseDatos, 0, NULL, 0) == NULL) {
	    std::cerr << "Error al conectar a la base de datos: " << mysql_error(conn) << std::endl;
	    mysql_close(conn);
	    return NULL;
	}
	
	return conn;
}

//Funcion para insertar estudiantes
void insertarEstudiantes(MYSQL* conn) {
    char consulta[200];

    int cantidadEstudiantes;
    std::cout << "Ingrese la cantidad de estudiantes que desee agregar a la base de datos: ";
    std::cin >> cantidadEstudiantes;

    for (int i = 0; i < cantidadEstudiantes; ++i) {
        int contadorEstudiantes = i + 1;
        std::cout << "Estudiante " << contadorEstudiantes << std::endl;

        std::string identificacion, nombre, apellido, grado;
        std::cout << "Identificacion del estudiante: ";
        std::cin >> identificacion;
        std::cout << "Nombre del estudiante: ";
        std::cin >> nombre;
        std::cout << "Apellido del estudiante: ";
        std::cin >> apellido;
        std::cout << "Grado del estudiante: ";
        std::cin >> grado;

        // Construir y ejecutar la consulta para obtener el ID del grado
        std::string consultaGrado = "SELECT id FROM tblgrado WHERE gradoCursado = '" + grado + "'";
        if (mysql_query(conn, consultaGrado.c_str())) {
            std::cerr << "Error al ejecutar la consulta: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            exit(1);
        }

        MYSQL_RES* res = mysql_store_result(conn);
        MYSQL_ROW row = mysql_fetch_row(res);
        int consultaGradoID = -1; // Inicializado con un valor que indica que no se encontró el grado

        if (row) {
            consultaGradoID = std::stoi(row[0]);
        } else {
            std::cout << "Grado no encontrado." << std::endl;
        }

        // Construir y ejecutar la consulta para insertar datos
        snprintf(consulta, sizeof(consulta), "INSERT INTO tbldatospersonales (identificacion, nombre, apellido, grado) VALUES ('%s', '%s', '%s', %d)",
                 identificacion.c_str(), nombre.c_str(), apellido.c_str(), consultaGradoID);

        if (mysql_query(conn, consulta)) {
            std::cerr << "Error al insertar datos: " << mysql_error(conn) << std::endl;
            mysql_close(conn);
            exit(1);
        }

        std::cout << "Estudiante " << contadorEstudiantes << " insertado correctamente" << std::endl;
    }
}

// Función para consultar estudiantes
void consultarEstudiantes(MYSQL* conn) {
    
    std::string identificacion;
    int opcion;
	std::cout << "¿Desea ver a algun estudiante en especifico? Digite 1 si es asi, de lo contrario digite 2: ";
	while (!(std::cin >> opcion)) {
	    std::cin.clear();
	    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
	    std::cout << "Entrada invalida. Intente de nuevo: ";
    }
	
	if (opcion == 1){
		std::string identificacion;
		std::cout << "Digite numero de identificacion del estudiante" << std::endl;
		std::cin >> identificacion;
    	std::string consulta = "SELECT * FROM tbldatospersonales WHERE identificacion = '" + identificacion + "'";
		if (mysql_query(conn, consulta.c_str())) {
		    std::cerr << "Error al ejecutar la consulta: " << mysql_error(conn) << std::endl;
		    return;
		}
    	// Obtener los resultados
    	MYSQL_RES* res = mysql_store_result(conn);
		// Verificar si se encontró una fila
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row) {
		    // Imprimir los datos de la fila encontrada
		    std::cout << "Identificacion: " << row[1] << std::endl
          				<< "Nombre: " << row[2] << std::endl
          				<< "Apellido: " << row[3] << std::endl
          				<< "Grado: " << row[4] << std::endl;
		} else {
		    std::cout << "No se encontro ningun estudiante con la identificacion proporcionada." << std::endl;
		}
	}
	
	if (opcion == 2){
		// Realizar la consulta a la base de datos
	    if (mysql_query(conn, "SELECT * FROM tbldatospersonales")) {
	        std::cerr << "Error al ejecutar la consulta: " << mysql_error(conn) << std::endl;
	        return;
	    }
	    MYSQL_RES* res = mysql_store_result(conn);
	    // Imprimir los resultados
	    std::cout << "Resultados de la consulta:" << std::endl;
	    while (MYSQL_ROW row = mysql_fetch_row(res)) {
	        std::cout << " Identificacion: " << row[1] << ", Nombre: " << row[2] << ", Apellido: " << row[3] << ", Grado: " << row[4] << std::endl;
	    }
	}
	
}

//Funcion para eliminar estudiantes
void eliminarEstudiantes(MYSQL* conn){	
	std::string identificacion;
	std::system("cls");
	std::cout << "(Si desea volver atras, digite 'back')"<<std::endl;
	std::cout << "Digite numero de identificacion del estudiante: ";
	std::cin >> identificacion;
		
	std::string eliminar = "DELETE FROM tbldatospersonales WHERE identificacion = '" + identificacion + "'";	
	if (mysql_query(conn, eliminar.c_str())) {
        std::cerr << "Error al eliminar estudiante: " << mysql_error(conn) << std::endl;
        return;
    }
	else{
    	std::cout << "Estudiante eliminado correctamente." << std::endl;    	
	}
  	
}

void actualizarEstudiantes(MYSQL* conn){
	std::string identificacion, nombre, apellido, nuevoGrado;
	int opcion;
	bool bandera=true;
	std::cout << "Digite la identificacion del estudiante: ";
	std::cin >> identificacion;
    std::string consulta = "SELECT identificacion FROM tbldatospersonales WHERE identificacion = '" + identificacion + "'";
    
	if (mysql_query(conn, consulta.c_str())) {
		std::cerr << "Error al ejecutar la consulta: " << mysql_error(conn) << std::endl;
		return;
	}

    MYSQL_RES* res = mysql_store_result(conn);
	MYSQL_ROW row = mysql_fetch_row(res);	
	if (row) {
            identificacion = row[0];
    } else {
            std::cout << "Estudiante no encontrado." << std::endl;
    }
    
    while(bandera){
    	std::cout << "Estudiante con identificacion " + identificacion + " encontrado" << std::endl;
		std::cout << "1. Identificacion\n"
	                 "2. Nombre\n"
					 "3. Apellido\n"
					 "4. Grado\n"
				  	 "5. Salir\n";
		std::cout << "Que desea actualizar del estudiante? Digite el numero segun la opcion: ";
		
		while (!(std::cin >> opcion)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << "Entrada invalida. Intente de nuevo: ";
        }
		
		switch(opcion){
			case 1: { 
				std::cout << "Digite nueva identificacion del estudiante: ";
				std::cin >> identificacion;
				std::string actualizacion = "UPDATE tbldatospersonales SET identificacion = '" + identificacion + "' WHERE identificacion = '" + identificacion + "'";
				if (mysql_query(conn, actualizacion.c_str())) {
        			std::cerr << "Error al actualizar datos: " << mysql_error(conn) << std::endl;
			    } else {
			        std::cout << "Datos actualizados correctamente." << std::endl;
			    }
				break;
			}
			case 2:{
				std::cout << "Digite nuevo nombre del estudiante: ";
				std::cin >> nombre;
				std::string actualizacion = "UPDATE tbldatospersonales SET nombre = '" + nombre + "' WHERE nombre = '" + nombre + "'";
				if (mysql_query(conn, actualizacion.c_str())) {
        			std::cerr << "Error al actualizar datos: " << mysql_error(conn) << std::endl;
			    } else {
			        std::cout << "Datos actualizados correctamente." << std::endl;
			    }
				break;
			}
			case 3:{
				std::cout << "Digite nuevo apellido del estudiante: ";
				std::cin >> apellido;
				std::string actualizacion = "UPDATE tbldatospersonales SET apellido = '" + apellido + "' WHERE apellido = '" + apellido + "'";
				if (mysql_query(conn, actualizacion.c_str())) {
        			std::cerr << "Error al actualizar datos: " << mysql_error(conn) << std::endl;
			    } else {
			        std::cout << "Datos actualizados correctamente." << std::endl;
			    }		
				break;
			}
			case 4:{
				std::cout << "Digite nuevo grado del estudiante: ";
				std::cin >> nuevoGrado;
				
				//Hallar ID del nuevo grado
       			std::string consultaGrado = "SELECT id FROM tblgrado WHERE gradoCursado = '" + nuevoGrado + "'";
        		if (mysql_query(conn, consultaGrado.c_str())) {
            		std::cerr << "Error al ejecutar la consulta: " << mysql_error(conn) << std::endl;
		            mysql_close(conn);
		            exit(1);
        		}
		        MYSQL_RES* res = mysql_store_result(conn);
		        MYSQL_ROW row = mysql_fetch_row(res);
		        int consultaGradoID = -1;
		        if (row) {
		            consultaGradoID = std::stoi(row[0]);
		        } else {
		            std::cout << "Grado no encontrado." << std::endl;
		        }
				
				//Hallar ID del antiguo grado del estudiante
				std::string consultaDos = "SELECT grado FROM tbldatospersonales WHERE identificacion = '" + identificacion + "'";
        		if (mysql_query(conn, consultaDos.c_str())) {
            		std::cerr << "Error al ejecutar la consulta: " << mysql_error(conn) << std::endl;
		            mysql_close(conn);
		            exit(1);
        		}
        		
		        MYSQL_RES* respuesta = mysql_store_result(conn);
		        MYSQL_ROW rowDos = mysql_fetch_row(respuesta);
		        int grado = -1;  // Variable para almacenar el grado como entero
				if (rowDos) {
				    grado = std::stoi(rowDos[0]);
				} else {
				    std::cout << "Grado no encontrado." << std::endl;
				}			
				
				std::string actualizacion = "UPDATE tbldatospersonales SET grado = " + std::to_string(consultaGradoID) + " WHERE grado = " + std::to_string(grado);
				if (mysql_query(conn, actualizacion.c_str())) {
        			std::cerr << "Error al actualizar datos: " << mysql_error(conn) << std::endl;
			    } else {
			        std::cout << "Datos actualizados correctamente." << std::endl;
			    }				
				break;
			}
			case 5: {
				bandera = false;
				break;
			}
			default: 
				break;	
		}
	}


	
}

int main(int argc, char** argv) {
    // Configuración de la conexión a la base de datos
    const char* host = "localhost";
    const char* usuario = "root";
    const char* contrasena = "";
    const char* baseDatos = "estudiantes";

    // Establecer la conexión a la base de datos
    MYSQL* conn = establecerConexion(host, usuario, contrasena, baseDatos);

    std::string nombreUsuario;
    std::string password;

    std::cout << "Digite su nombre de usuario: ";
    std::cin >> nombreUsuario;

    std::cout << "Digite su contrasena: ";
    std::cin >> password;

    if (nombreUsuario == "admin" && password == "admin") {
        std::cout << "Bienvenido " << nombreUsuario << " al gestor de estudiantes del colegio Garces Navas I.E.D.\n";
        bool bandera = true;

        while (bandera) {
            std::cout << "1. Consultar estudiantes\n"
                         "2. Insertar estudiantes\n"
                         "3. Actualizar estudiantes\n"
                         "4. Eliminar estudiantes\n"
                         "5. Salir\n";

            int opcion;

            std::cout << "Digite el numero segun la opcion que desea: " <<std::endl;

            // Si la opcion es incorrecta, descarta lo digitado 
            while (!(std::cin >> opcion)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                std::cout << "Entrada invalida. Intente de nuevo: ";
            }

            switch (opcion) {
                case 1:
                    consultarEstudiantes(conn);
                    break;
                case 2:
                    insertarEstudiantes(conn);                 
                    break;
                case 3:
                    actualizarEstudiantes(conn);
                    break;
                case 4: 
                    eliminarEstudiantes(conn);
                    break;
                case 5:
                    std::cout << "Gracias por usar el programa :)\n";
                    mysql_close(conn);
                    bandera = false;
            }
        }
    } else {
        std::cout << "Acceso denegado, usuario o contrasena incorrectos";
    }
    return 0;
}		