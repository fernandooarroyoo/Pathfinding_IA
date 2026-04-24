#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoIngenieroNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoIngenieroNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoIngenieroNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoIngenieroNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoIngenieroNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoIngenieroNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoIngenieroNivel_6(sensores);
    break;
  }

  return accion;
}

/**
 * @brief Determina si casilla viable por altura
 * @param casilla tipo de terreno
 * @param dif diferencia de altura entre casillas
 * @param zap indica si estoy en posesión de las zapatillas
 * @return 'P' si no es accesible por altura y casilla en otro caso
 */
char ViablePorAlturaI(char casilla, int dif, bool zap)
{
  if (abs(dif) <= 1 or (zap and abs(dif) <= 2))
  {
    return casilla;
  }
  else
  {
    return 'P';
  }
}

/**
 * @brief Determina la mejor opcion entre las 3 casillas que tiene delante
 * @param i terreno que hay en la posición 1 de superficie (45 izq)
 * @param c terreno que hay en la posición 2 de superficie (justo delante)
 * @param d terreno que hay en la posición 3 de superficie (45 derecha)
 * @return 2 si es mejor WALK, 1 para TURN_SL y 3 para TURN_SR. 0 no hay nada interesante
 */
int VeoCasillaInteresanteI(char i, char c, char d, bool zapatillas)
{
  if (c == 'U')
    return 2;
  else if (i == 'U')
    return 1;
  else if (d == 'U')
    return 3;
  else if (!zapatillas)
  {
    if (c == 'D')
      return 2;
    else if (i == 'D')
      return 1;
    else if (d == 'D')
      return 3;
  }
  if (c == 'C')
    return 2;
  else if (i == 'C')
    return 1;
  else if (d == 'C')
    return 3;
  else
    return 0;
}

// Niveles iniciales (Comportamientos reactivos simples)
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
  Action accion = IDLE;
  // El comportamiento de seguir un camino hasta encontrar una planta de T. Residuos
  // Poner el valor de los sensores de visión sobre los mapas.
  ActualizarMapa(sensores);

  // Compruebo si está en medio de un giro forzado, por esquivar a un técnico, por ejemplo.
  if (giros_forzados > 0)
  {
    giros_forzados--;
    accion = TURN_SL;
    last_action = accion;
    return accion;
  }

  // Actualización de las variables de estado
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  // Definición del comportamiento
  if (sensores.superficie[0] == 'U')
  { // llegue a una 'U'
    return IDLE;
  }

  char i = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  if (sensores.agentes[2] == 't')
  { // si se encuentra a un técnico de cara lo fuerzo a girar para ir por otro sitio
    giros_forzados = 1;
    accion = TURN_SL;
    last_action = accion;
    return accion;
  }

  int pos = VeoCasillaInteresanteI(i, c, d, tiene_zapatillas);

  switch (pos)
  {
  case 2:
    accion = WALK;
    break;
  case 1:
    accion = TURN_SL;
    break;
  case 3:
    accion = TURN_SR;
    break;
  default:
    accion = TURN_SL;
    break;
  }

  // Devolver la siguiente acción a hacer
  last_action = accion;
  return accion;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

ubicacion ComportamientoIngeniero::Izquierda(const ubicacion &actual) const
{
  // vamos a aprovechar la función Delante que tenemos, voy a calcular el rumbo
  // Nuevo, sabiendo que girar 90º es restarle 2 (en mod 8, porque se divide en giros de 45º)
  // El casteo cambia 0 por norte, 1 noroeste...
  ubicacion izq = {actual.f, actual.c, actual.brujula};
  izq.brujula = static_cast<Orientacion>((actual.brujula + 6) % 8); // +6 = -2 mod 8
  return Delante(izq);
}

ubicacion ComportamientoIngeniero::Derecha(const ubicacion &actual) const
{
  ubicacion der = {actual.f, actual.c, actual.brujula};
  der.brujula = static_cast<Orientacion>((actual.brujula + 2) % 8);
  return Delante(der);
}

/**
 * @brief Comportamiento reactivo del ingeniero para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
  // TODO: Implementar comportamiento reactivo para el Nivel 1.
  Action accion = IDLE;
  ActualizarMapa(sensores);
  if (giros_forzados > 0)
  {
    giros_forzados--;
    accion = TURN_SL;
    last_action = accion;
    return accion;
  }

  // Actualización de las variables de estado
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  char i = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas); // hipótesis: los caminos pueden cambiar de cota

  ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
  ubicacion Alante = Delante(actual);

  vector<pair<ubicacion, Action>> candidatas;

  if (c == 'C' || c == 'S' || c == 'U' || c == 'D' || c == 'X')
    candidatas.push_back({Alante, WALK});
  if (i == 'C' || i == 'S' || i == 'U' || i == 'D' || i == 'X')
    candidatas.push_back({Izquierda(actual), TURN_SL});
  if (d == 'C' || d == 'S' || d == 'U' || d == 'D' || i == 'X')
    candidatas.push_back({Derecha(actual), TURN_SR});

  if (sensores.agentes[2] == 't')
  { // si se encuentra a un técnico de cara lo fuerzo a girar para ir por otro sitio
    giros_forzados = 2;
    accion = TURN_SL;
    last_action = accion;
    return accion;
  }

  // Para elegir la de menos visitas
  int menor = 2147483647; // maximo valor de int
  for (auto x : candidatas)
  {
    if (mapa_visitado[{x.first.f, x.first.c}] < menor)
    {
      accion = x.second;
      menor = mapa_visitado[{x.first.f, x.first.c}];
    }
    else if (mapa_visitado[{x.first.f, x.first.c}] == menor && x.second == WALK)
    {
      accion = x.second;
    }
  }
  if (candidatas.empty())
  {
    giros_forzados = 1;
    accion = TURN_SL;
    last_action = accion;
    return accion;
  }

  // se actualiza la casilla visitada en función de la acción que se vaya a
  ubicacion destino = Alante;
  if (accion == TURN_SL)
    destino = Izquierda(actual);
  else if (accion == TURN_SR)
    destino = Derecha(actual);
  mapa_visitado[{destino.f, destino.c}]++;

  last_action = accion;
  return accion;
}

// Niveles avanzados (Uso de búsqueda)

// Funciones necesarias para la busqueda en anchura.

EstadoI NextCasillaIngeniero(const EstadoI &st)
{
  EstadoI siguiente = st;
  switch (st.site.brujula)
  {
  case norte:
    siguiente.site.f = st.site.f - 1;
    break;
  case noreste:
    siguiente.site.f = st.site.f - 1;
    siguiente.site.c = st.site.c + 1;
    break;
  case este:
    siguiente.site.c = st.site.c + 1;
    break;
  case sureste:
    siguiente.site.f = st.site.f + 1;
    siguiente.site.c = st.site.c + 1;
    break;
  case sur:
    siguiente.site.f = st.site.f + 1;
    break;
  case suroeste:
    siguiente.site.f = st.site.f + 1;
    siguiente.site.c = st.site.c - 1;
    break;
  case oeste:
    siguiente.site.c = st.site.c - 1;
    break;
  case noroeste:
    siguiente.site.f = st.site.f - 1;
    siguiente.site.c = st.site.c - 1;
  }
  return siguiente;
}

bool CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  EstadoI next = NextCasillaIngeniero(st);

  bool transitable = terreno[next.site.f][next.site.c] != 'P' and terreno[next.site.f][next.site.c] != 'M' and
                     terreno[next.site.f][next.site.c] != 'B';

  int dif_altura = abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]);
  bool check_altura = st.zapatillas ? (dif_altura <= 2) : (dif_altura <= 1);

  return transitable && check_altura;
}

EstadoI applyT(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
 EstadoI next = st;
  EstadoI intermedia; //para comprobar si el salto es válido
  int dif_altura;
  
  switch (accion)
  {
  case WALK:
    if (CasillaAccesibleIngeniero(st, terreno, altura))
    {
      next = NextCasillaIngeniero(st);
    }
    break;

  case JUMP:
    //casilla intermedia
    intermedia = NextCasillaIngeniero(st);
    
    //no podemos saltar en medio de un precipicio, por ejemplo
    if (terreno[intermedia.site.f][intermedia.site.c] != 'P' && 
        terreno[intermedia.site.f][intermedia.site.c] != 'M' && 
        terreno[intermedia.site.f][intermedia.site.c] != 'B') 
    {
       
        EstadoI destino = NextCasillaIngeniero(intermedia);
        
        bool transitable_destino = terreno[destino.site.f][destino.site.c] != 'P' && 
                                   terreno[destino.site.f][destino.site.c] != 'M' && 
                                   terreno[destino.site.f][destino.site.c] != 'B';
                                   
       
        dif_altura = abs(altura[destino.site.f][destino.site.c] - altura[st.site.f][st.site.c]);
        bool check_altura_destino = st.zapatillas ? (dif_altura <= 2) : (dif_altura <= 1);
        
        if (transitable_destino && check_altura_destino) {
            next = destino; 
        }
    }
    break;

  case TURN_SR:
    next.site.brujula = (Orientacion)((next.site.brujula + 1) % 8);
    break;
    
  case TURN_SL:
    next.site.brujula = (Orientacion)((next.site.brujula + 7) % 8);
    break;
  }
  
  return next;
}

bool Find(const NodoI &st, const list<NodoI> &lista)
{
  auto it = lista.begin();
  while (it != lista.end() and !((*it) == st))
  {
    it++;
  }
  return (it != lista.end());
}

list<Action> ComportamientoIngeniero::B_Anchura(const EstadoI &inicio, const EstadoI &final, const vector<vector<unsigned char>> &terreno, vector<vector<unsigned char>> &altura)
{
  NodoI current_node;
  queue<NodoI> frontier;
  set<NodoI> explored;

  current_node.estado = inicio;

  // comprobacion inicial
  if (inicio.site.f == final.site.f && inicio.site.c == final.site.c)
  {
    return list<Action>();
  }
  // comprobamos si nacemos en unas zapatillas
  if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D')
  {
    current_node.estado.zapatillas = true;
  }

  frontier.push(current_node);
  explored.insert(current_node);

  // Las 4 acciones permitidas en el lv2
  Action posibles_acciones[] = {WALK, JUMP, TURN_SR, TURN_SL};

  while (!frontier.empty())
  {
    current_node = frontier.front();
    frontier.pop();

    for (Action accion : posibles_acciones)
    {
      NodoI child = current_node;

      child.estado = applyT(accion, current_node.estado, terreno, altura);

      // si el hijo no ha hecho nada, lo ignoro
      if (child.estado.site.f == current_node.estado.site.f and
          child.estado.site.c == current_node.estado.site.c and
          child.estado.site.brujula == current_node.estado.site.brujula)
      {
        continue;
      }

      // compruebo si estoy en un sitio de zapatillas
      if (terreno[child.estado.site.f][child.estado.site.c] == 'D')
      {
        child.estado.zapatillas = true;
      }

      // registramos en la secuencia
      child.secuencia.push_back(accion);

      // compruebo si he llegado a la meta
      if (child.estado.site.f == final.site.f && child.estado.site.c == final.site.c)
      {
        return child.secuencia; // ¡Encontrado el camino más corto en instantes!
      }

      // miro si esta explorado, y lo meto en la cola frontier
      if (explored.find(child) == explored.end())
      {
        explored.insert(child);
        frontier.push(child);
      }
    }
  }

  // si no hemos encontrado nada, devolvemos una lista vacia
  return list<Action>();
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (búsqueda).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
  // TODO: Implementar búsqueda para el Nivel 2.
  Action accion = IDLE;
  if (!hayPlan)
  {
    EstadoI inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;
    plan = B_Anchura(inicio, fin, mapaResultado, mapaCotas);
    VisualizaPlan(inicio.site, plan);
    hayPlan = plan.size() != 0;
  }

  if (hayPlan and plan.size() > 0)
  {
    accion = plan.front();
    plan.pop_front();
  }
  if (plan.size() == 0)
  {
    hayPlan = false;
  }
  return accion;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  return IDLE;
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
        mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
      }
    break;
  case noreste:
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
    mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
    mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
    mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
    mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
    mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
    mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
    mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
    mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
    break;
  case este:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
        mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
      }
    break;
  case sureste:
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
    mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
    mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
    mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
    mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
    mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
    mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
    mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
    mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
    mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
    mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
    break;
  case sur:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
        mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
      }
    break;
  case suroeste:
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
    mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
    mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
    mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
    mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
    mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
    mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
    mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
    mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
    break;
  case oeste:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
        mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
      }
    break;
  case noroeste:
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
    mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
    mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
    mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
    mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
    mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
    mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
    mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
    mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
    mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
    mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
    break;
  }
}

/**
 * @brief Determina si una casilla es transitable para el ingeniero.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable (no es muro ni precipicio).
 */
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el ingeniero: desnivel máximo 1 sin zapatillas, 2 con zapatillas.
 * @param actual Estado actual del agente (fila, columna, orientacion, zap).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}

/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

/**
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 *
 * @param plan  Lista de pasos (fila, columna, operación),
 *              donde operacion = -1 (DIG), operación = 1 (RAISE).
 */
void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
  listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion)(((int)cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion)(((int)cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}

/**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 *
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
