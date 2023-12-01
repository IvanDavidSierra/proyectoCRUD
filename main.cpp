#include <iostream>
#include <mysql.h>
#include <string>
#include <limits>
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


int main(int argc, char** argv) {
	// Configuración de la conexión a la base de datos
    const char* host = "localhost";
    const char* usuario = "root";
    const char* contrasena = "";
    const char* baseDatos = "estudiantes";
    // Establecer la conexión a la base de datos
    MYSQL* conn = establecerConexion(host, usuario, contrasena, baseDatos);
    
    std::string nombreUsuario;
    std::cout << "Digite su nombre de usuario: ";
    std::getline(std::cin, nombreUsuario);
    std::cout << "Bienvenido " << nombreUsuario << " al gestor de estudiantes del colegio Garces Navas I.E.D.\n";
    bool bandera = true;

    while (bandera) {
        std::cout << "1. Consultar estudiantes\n"
					 "2. Insertar estudiantes\n"
                     "3. Actualizar estudiantes\n"
                     "4. Eliminar estudiantes\n"
                     "5. Salir\n";

        int opcion;
        std::cout << "Digite el numero segun la opcion que desea: ";
        
        //Si la opcion es incorrecta, descarta lo digitado 
        while (!(std::cin >> opcion)) {
	        std::cin.clear();
	        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
	        std::cout << "Entrada invalida. Intente de nuevo: ";
    	}

        switch (opcion) {
            case 1:
                //consultarEstudiantes();
                break;
            case 2:
                insertarEstudiantes(conn);
                break;
            case 3:
                //actualizarEstudiantes();
                break;
            case 4: 
            	//eliminarEstudiantes();
            	break;
            case 5:
                std::cout << "Gracias por usar el programa :)\n";
                mysql_close(conn);
                bandera = false;
            default:
            	std::cout << "Opcion invalida, intente de nuevo";
            	break;
        }
    }   
	return 0;
}


	
	
	
		