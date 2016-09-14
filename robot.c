/*
Copyright (c) 2002, Andrew Howard
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the Player Project nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <time.h>

#include <libplayerc/playerc.h>

#define pi 3.141590


/***************************************** VARIABLES GLOBALES **********************************************/
typedef struct
{
	float dist_x_left;
	float dist_x_right;
	float dist_y_up;
	float dist_y_down;
}Distancias;

  playerc_client_t *client;
  playerc_position2d_t *position2d;
  playerc_sonar_t *sonar;

	int mapa_casillas[25][4] =	{	{1100, 1001, 11, 110}, {1002, 12, 120, 1200}, {1100, 1001, 11, 110}, {1, 10, 100, 1000}, {1000,1,10, 100},
																{10, 100, 1000, 1}, {1210, 2101, 1012, 121}, {211, 2110, 1102, 1021}, {2102, 1022, 221, 2210}, {12, 120, 1200, 2001},
																{1200, 2001, 12, 120}, {121, 1210, 2101, 1012}, {2, 20, 200, 2000}, {1010, 101, 1010, 101}, {1000, 1, 10, 100}, 
																{112, 1120, 1201, 2011}, {1002, 21, 210, 2100}, {1200, 2001, 12, 120}, {121, 1210, 2101, 1012}, {12, 120, 1200, 2001},
																{100, 1000, 1, 10}, {11, 110, 1100, 1001}, {210, 2100, 1002, 21}, {101, 1010, 101, 1010}, {2, 20, 200, 2000}
															};

	int rutas[25][2] =	{	{1,1}, {6,0}, {7,0}, {2,3}, {9,0},
												{0,2}, {11,0}, {6,3}, {7,3}, {8,3},
												{15,0}, {10,3}, {11,3}, {8,2}, {19,0},
												{25,3}, {15,3}, {18,1}, {13,2}, {18,3},
												{21,1}, {16,2}, {17,2}, {22,3}, {23,3}
											};

	int casillas_adyacentes[25][4] = {	{5,1,-1,-1}, {6,-1,-1, 0}, {7,3,-1,-1}, {-1,-1,-1,2}, {9,-1,-1,-1}, 
																			{-1,-1,0,-1}, {11,7,1,-1}, {-1,8,-1,-6}, {13,9,-1,7}, {-1,-1,4,8},
																			{15,11,-1,-1}, {-1,12,6,10}, {-1,-1,-1,11}, {18,-1,8,-1}, {19,-1,-1,-1},																		
																			{-1,16,10,25}, {21,-1,-1, 15}, {22,18,-1,-1}, {-1,19,13,17}, {-1,-1,14,18},																			
																			{-1,21,-1,-1}, {-1,-1,16,20}, {-1,23,17,-1}, {-1,24,-1,22}, {-1,-1,-1,23}
																		};

	int candidatos_casilla_inicial[25][4] = { {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1},
																						{-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1},
																						{-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1},
																						{-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1},
																						{-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}, {-1,-1,-1,-1}
																					};


	int candidatos_casilla_inicial_siguiente[25][2] = {	{-2,-2}, {-2,-2}, {-2,-2}, {-2,-2}, {-2,-2},
																											{-2,-2}, {-2,-2}, {-2,-2}, {-2,-2}, {-2,-2},
																											{-2,-2}, {-2,-2}, {-2,-2}, {-2,-2}, {-2,-2},
																											{-2,-2}, {-2,-2}, {-2,-2}, {-2,-2}, {-2,-2},
																											{-2,-2}, {-2,-2}, {-2,-2}, {-2,-2}, {-2,-2} 
																										};
/********************************************************************************************************************/



/************************************************** FUNCIONES *******************************************************/
/********************************************************************************************************************/
int giro_direccion (int direccion_inicial, int direccion_final)
{
	float angulo_inicial = 0;
	float angulo_final = 0;
	float angulo_actual = 0;
	float variacion_angulo = 0;
	int posicion_alcanzada = 0;
	int variacion_direccion = 0;

	printf("\n\nFuncion Giro_Direccion\n");

	//Leer la odometría
	playerc_client_read(client);
	//Guardamos la posición actual
	angulo_actual = position2d->pa; //Posición ángulo
	printf("Angulo Actual : %f\n", angulo_actual);

	variacion_direccion = direccion_final - direccion_inicial;
	printf("Direccion Inicial: %d , Direccion Final: %d, Variacion Direccion: %d\n", direccion_inicial, direccion_final, variacion_direccion);
	switch (variacion_direccion)
	{
		case 0:
			variacion_angulo = 0;
			break;

		case 1:
			variacion_angulo = -(pi/2);
			break;

		case 2:
			variacion_angulo = pi;
			break;

		case 3:
			variacion_angulo = (pi/2);
			break;

		case -1:
			variacion_angulo = (pi/2);
			break;

		case -2:
			variacion_angulo = pi;
			printf("Variacion direcion -2\n");
			break;

		case -3:
			variacion_angulo = -(pi/2);
			break;

		default:
			break;
	}
	
	angulo_final = angulo_actual + variacion_angulo;

	printf("Giramos desde el angulo %f hasta el angulo %f\n", angulo_actual, angulo_final);
	playerc_position2d_set_odom(position2d, 0, 0, angulo_inicial);

	if (variacion_angulo > 0)
	{
		// Se giran hasta alcanzar el angulo final en sentido horario
		playerc_position2d_set_cmd_vel(position2d, 0, 0, 0.3, 1);
	}
	else if (variacion_angulo < 0)
	{
		// Se giran hasta alcanzar el angulo final en sentido horario
		playerc_position2d_set_cmd_vel(position2d, 0, 0, -0.3, 1);
	}


	posicion_alcanzada = 0;
	while(posicion_alcanzada==0)
	{
		//Leer la odometría
		playerc_client_read(client);
		//Guardamos la posición actual
		angulo_actual=position2d->pa; //Posición ángulo
		//printf("Angulo : %f\n",position2d->pa);
                 
		//Comprobación se si se ha alcanzado el angulo destino
		if ( (angulo_actual >= ( angulo_final - 0.05) ) && (angulo_actual <= ( angulo_final + 0.05)))
		{
			posicion_alcanzada=1;
			// Imprimimos la posición final
    	printf("Posición Alcanzada Girando : %f %f %f\n",
    	position2d->px, position2d->py, position2d->pa);
			// Se para el giro del robot
 			playerc_position2d_set_cmd_vel(position2d,0,0,0,1);
		}	    
	}
	return 1;

}
/********************************************************************************************************************/





// 
int calcular_codigo_casilla()
{
	float distancia_delante, distancia_derecha, distancia_detras, distancia_izquierda = 0;
	float pos_a_actual=0;
	float pos_a_final=0;
	int posicion_alcanzada = 0;
	int a = 0;
	int codigo_casilla = 0; 

	printf("\n\nFuncion Codigo_Casilla\n");

	// Wait for new data from server
  playerc_client_read(client);
	playerc_sonar_get_geom(sonar); 

  // Print current robot pose
  printf("Posición Actual : %f %f %f\n",
        position2d->px, position2d->py, position2d->pa);
		
		
	printf("Lectura del Sonar: ");
	for (a=0;a<8;a++)
	{		
		printf( ", %f",sonar->scan[a]);
	}
	printf("\n");

	pos_a_actual=position2d->pa; //Posición ángulo
	distancia_izquierda = sonar->scan[0];
	distancia_derecha = sonar->scan[5];


	// Se giran 90º en sentido antihorario
	playerc_position2d_set_cmd_vel(position2d, 0, 0, 0.3, 1);
	pos_a_final = pos_a_actual + (pi/2);
	posicion_alcanzada = 0;	
	while(posicion_alcanzada==0)
	{
		//Leer la odometría
		playerc_client_read(client);
		//Guardamos la posición actual
		pos_a_actual=position2d->pa; //Posición ángulo
		//printf("angulo : %f\n",position2d->pa);
                 
		//Comprobación se si se ha alcanzado el angulo destino
		if ( (pos_a_actual >= ( pos_a_final - 0.05)) && (pos_a_actual <= ( pos_a_final + 0.05)))
		{
			posicion_alcanzada=1;
			// Imprimimos la posición final
    	printf("Posición Alcanzada : %f %f %f\n",
    	position2d->px, position2d->py, position2d->pa);
 			playerc_position2d_set_cmd_vel(position2d,0,0,0,1);
		}	    
	}

	// Wait for new data from server
	playerc_client_read(client);
	playerc_sonar_get_geom(sonar); 

	printf("Lectura del Sonar girado: ");
	for (a=0;a<8;a++)
	{		
		printf( ", %f",sonar->scan[a]);
	}
	printf("\n");

	pos_a_actual=position2d->pa; //Posición ángulo
	distancia_detras = sonar->scan[0];
	distancia_delante = sonar->scan[5];

	// Se giran -90º en sentido horario para volver a la posicion inicial
	playerc_position2d_set_cmd_vel(position2d, 0, 0, -0.3, 1);
	pos_a_final = pos_a_actual - (pi/2);
	posicion_alcanzada=0;	
	while(posicion_alcanzada==0)
	{
		//Leer la odometría
		playerc_client_read(client);
		//Guardamos la posición actual
		pos_a_actual=position2d->pa; //Posición ángulo
		//printf("angulo : %f\n",position2d->pa);
                 
		//Comprobación se si se ha alcanzado el angulo destino
		if ( (pos_a_actual >= ( pos_a_final - 0.05)) && (pos_a_actual <= ( pos_a_final + 0.05)))
		{
			posicion_alcanzada=1;
			// Imprimimos la posición final
    	printf("\n\nPosición Alcanzada : %f %f %f\n",
    	position2d->px, position2d->py, position2d->pa);
 			playerc_position2d_set_cmd_vel(position2d,0,0,0,1);
		}	    
	}

	
	printf("\nDistancias de la casilla | %f | %f | %f | %f | \n", distancia_delante, distancia_derecha, distancia_detras, distancia_izquierda);

	if( (distancia_delante >= 0.0) && (distancia_delante < 3.0) )
		codigo_casilla = codigo_casilla;
	else if ( (distancia_delante >= 3.0) && (distancia_delante < 5.0))
		codigo_casilla = codigo_casilla + 1000;
	else if ( distancia_delante >= 5.0)
		codigo_casilla = codigo_casilla + 2000;

	if( (distancia_derecha >= 0.0) && (distancia_derecha < 3.0) )
		codigo_casilla = codigo_casilla;
	else if ( (distancia_derecha >= 3.0) && (distancia_derecha < 5.0))
		codigo_casilla = codigo_casilla + 100;
	else if ( distancia_derecha >= 5.0)
		codigo_casilla = codigo_casilla + 200;

	if( (distancia_detras >= 0.0) && (distancia_detras < 3.0) )
		codigo_casilla = codigo_casilla;
	else if ( (distancia_detras >= 3.0) && (distancia_detras < 5.0))
		codigo_casilla = codigo_casilla + 10;
	else if ( distancia_detras >= 5.0)
		codigo_casilla = codigo_casilla + 20;

	if( (distancia_izquierda >= 0.0) && (distancia_izquierda < 3.0) )
		codigo_casilla = codigo_casilla;
	else if ( (distancia_izquierda >= 3.0) && (distancia_izquierda < 5.0))
		codigo_casilla = codigo_casilla + 1;
	else if ( distancia_izquierda >= 5.0)
		codigo_casilla = codigo_casilla + 2;

	printf("Codigo de la Casilla Inicial= %d\n",codigo_casilla);
	return codigo_casilla;
}
/*************************************************************************************************/

/*************************************************************************************************/
int localizar_casilla_actual(int codigo_casilla)
{
	int j, i, z, a;
	//int candidatos[25][4] = {-1};
	float angulo_inicial, angulo_final;
	float pos_a_actual=0;
	float pos_y_actual=0;
	float pos_x_actual=0;
	int posicion_alcanzada = 0;
	int robot_localizado = 0;
	int giro_realizado = 0;
	int id_casilla_ini = 0;
	int direccion_movimiento = 0;

	printf("\n\nFuncion Localizar_Casilla_Actual\n");
	z = 0;
	for (i=0;i<25;i++)
	{
		for(j=0;j<4;j++)
		{
			if (mapa_casillas[i][j] == codigo_casilla)
			{
				printf("Posible casilla inicial: Casilla %d en direccion %d\n", i, j);
				candidatos_casilla_inicial[z][0] = i; // ID casilla Inicial
				candidatos_casilla_inicial[z][1] = j; // Direccion inicial
				candidatos_casilla_inicial[z][2] = rutas[i][0]; // ID siguiente casilla para la salida
				candidatos_casilla_inicial[z][3] = rutas[i][1]; // Direccion en la que hay que avanzar hasta alcanzar la sig casilla para la salida
				z++;
			}
		}
	}
	playerc_position2d_set_odom(position2d, 0, 0, 0);
	giro_direccion(candidatos_casilla_inicial[0][1], candidatos_casilla_inicial[0][3]);
	giro_realizado = candidatos_casilla_inicial[0][1] - candidatos_casilla_inicial[0][3];

	// Se avanzan 3 metros hasta la siguiente casilla
	playerc_position2d_set_odom(position2d, 0, 0, 0);
	playerc_position2d_set_cmd_vel(position2d, 0.3, 0, 0, 1);
	posicion_alcanzada = 0;
	while(posicion_alcanzada==0)
	{
		//Leer la odometría
		playerc_client_read(client);
		//Guardamos la posición actual
		pos_x_actual=position2d->px; //Posición en Y
		if (pos_x_actual < 0)
			pos_x_actual = pos_x_actual * (-1);

		//printf("Avance en X: %f\n",position2d->px);
		//printf("Avance en Y: %f\n",position2d->py);
                 
		//Comprobación se si se ha alcanzado el angulo destino
		if ( (pos_x_actual >= ( 3 - 0.05) ) && (pos_x_actual <= ( 3 + 0.05)))
		{
			posicion_alcanzada=1;
			// Imprimimos la posición final
    	printf("\n\nPosición Alcanzada Avanzando: %f %f %f\n",
    	position2d->px, position2d->py, position2d->pa);
 			playerc_position2d_set_cmd_vel(position2d,0,0,0,1);
		}	    
	}

	// Una vez hecho el movimiento, se calcula el codigo de la casilla actual
	a = calcular_codigo_casilla();

	printf ("Codigo de Segunda Casilla Inicial = %d\n", a);

	printf("Candidatos a Segunda Casilla Inicial\n");
	z = 0;
	for (i=0;i<25;i++)
	{
		for(j=0;j<4;j++)
		{
			if (mapa_casillas[i][j] == a)
			{
				printf("Posible casilla inicia: Casilla %d en direccion %d \n", i, j);
				candidatos_casilla_inicial_siguiente[z][0] = i;
				candidatos_casilla_inicial_siguiente[z][1] = j;
				z++;
			}

/*			else
			{
				printf("\n Casilla %d no es candidata inicial", mapa_casillas[i][j]);
			}  */
		}
	}

	// Se calcula en que casilla esta el robot
	for (i=0; i<25 && !robot_localizado; i++)
	{
		for(j=0; j<25 && !robot_localizado; j++)
		{
	//		if( (candidatos_casilla_inicial[i][2] == candidatos_casilla_inicial_siguiente[j][0]) &&
		//		(candidatos_casilla_inicial[i][3] == candidatos_casilla_inicial_siguiente[j][1]) )

			// Se comprueba si teorico movimiento está permitido
			id_casilla_ini = candidatos_casilla_inicial[i][0];
			direccion_movimiento = giro_realizado + candidatos_casilla_inicial[i][1];
			if (direccion_movimiento > 3)
			{	
				direccion_movimiento = direccion_movimiento % 4;
			}
			printf("Direccion Movimiento = %d\n", direccion_movimiento);
			if( casillas_adyacentes[id_casilla_ini][direccion_movimiento] == candidatos_casilla_inicial_siguiente[j][0] )
			{
	
				printf("\nROBOT LOCALIZADO en Casilla %d y direccion %d\n", candidatos_casilla_inicial_siguiente[j][0], candidatos_casilla_inicial_siguiente[j][1]);
				//robot_localizado = 1;
			}
			
		}

	}

	return 0;

}
/****************************************************************************************/


/**************************************** MAIN *********************************/
int main(int argc, const char **argv)
{
  int i, j, a;
	int casilla_actual = 0;

  // Create a client and connect it to the server.
  client = playerc_client_create(NULL, "localhost", 6665);
  if (0 != playerc_client_connect(client))
    return -1;

  // Create and subscribe to a position2d device.
  position2d = playerc_position2d_create(client, 0);
  if (playerc_position2d_subscribe(position2d, PLAYER_OPEN_MODE))
    return -1;

   // Create and subscribe the sonar device
   sonar = playerc_sonar_create(client, 0);
   if (playerc_sonar_subscribe(sonar, PLAYER_OPEN_MODE))
    return -1;

	// Se pone a 0 el odometro
  playerc_position2d_set_odom(position2d, 0.0, 0.0, 0.0);

	/************************ LOCALIZAR AL ROBOT ****************************/
	a = calcular_codigo_casilla();
	printf ("Codigo de Casilla Inicial = %d\n", a);

	printf("\nCandidatos a Casilla Inicial");
	for (i=0;i<25;i++)
	{
		for(j=0;j<4;j++)
		{
			if (mapa_casillas[i][j] == a)
			{
				printf("\nPosible casilla inicial: Casilla %d en direccion %d", i, j);
			}

/*			else
			{
				printf("\n Casilla %d no es candidata inicial", mapa_casillas[i][j]);
			} */

		}

	}

	casilla_actual = localizar_casilla_actual(a);
	/**************************************************************************/





  // Shutdown
  //playerc_position2d_unsubscribe(position2d);
 // playerc_position2d_destroy(position2d);
  //playerc_client_disconnect(client);
  //playerc_client_destroy(client);

  //initialize
/*playerc_graphics2d_t *graficos;
player_point_2d_t *puntos;
player_color_t color;
puntos=(player_point_2d_t *)malloc(sizeof(player_point_2d_t)*(1)); //(1) punto
color.red=255; color.green=0; color.blue=0;
// Create and subscribe to a graphics device
graficos = playerc_graphics2d_create(client, 0);
if (playerc_graphics2d_subscribe(graficos, PLAYER_OPEN_MODE) != 0)
{
	fprintf(stderr, "error: %s∖n", playerc_error_str());
return -1;
}
//fix colour
playerc_graphics2d_setcolor (graficos, color);
//clear screen
playerc_graphics2d_clear(graficos);
//draw current robot pose
puntos[0].px=position2d->px;
puntos[0].py=position2d->py;
playerc_graphics2d_draw_points (graficos, puntos, 1);
// Unsuscribe and Destroy
// graphics2d
playerc_graphics2d_unsubscribe(graficos); playerc_graphics2d_destroy(graficos);

*/
  return 0;
}