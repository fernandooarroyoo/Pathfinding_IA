#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================
/**
 * @brief Determina si casilla viable por altura
 * @param casilla tipo de terreno
 * @param dif diferencia de altura entre casillas
 * @param zap indica si estoy en posesión de las zapatillas
 * @return 'P' si no es accesible por altura y casilla en otro caso
 */
char ViablePorAlturaT(char casilla, int dif)
{
  if (abs(dif) <= 1)
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
int VeoCasillaInteresanteT(char i, char c, char d)
{
  if (c == 'U')
    return 2;
  else if (i == 'U')
    return 1;
  else if (d == 'U')
    return 3;
  else if (c == 'C')
    return 2;
  else if (i == 'C')
    return 1;
  else if (d == 'C')
    return 3;
  else
    return 0;
}

Action ComportamientoTecnico::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoTecnicoNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoTecnicoNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoTecnicoNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoTecnicoNivel_3(sensores);
    break;
  // case 3: accion = ComportamientoTecnicoNivel_E(sensores); break;
  case 4:
    accion = ComportamientoTecnicoNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoTecnicoNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoTecnicoNivel_6(sensores);
    break;
  }

  return accion;
}

// Niveles del técnico
Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
  Action accion = IDLE;
  // El comportamiento de seguir un camino hasta encontrar una planta de T. Residuos
  // Poner el valor de los sensores de visión sobre los mapas.
  ActualizarMapa(sensores);

  // Actualización de las variables de estado
  // if(sensores.superficie[0] == 'D') tiene_zapatillas = true;

  // Definición del comportamiento
  if (sensores.superficie[0] == 'U')
  { // lleue a una 'U'
    return IDLE;
  }

  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);
  int pos = VeoCasillaInteresanteT(i, c, d);

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
bool ComportamientoTecnico::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

int VeoCasillaInteresanteII(char i, char c, char d)
{

  if (c == 'C' || c == 'S')
    return 2;
  else if (i == 'C' || i == 'S')
    return 1;
  else if (d == 'C' || d == 'S')
    return 3;
  else
    return 0;
}
/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores)
{

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

  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]); // hipótesis: los caminos pueden cambiar de cota

  ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
  ubicacion Alante = Delante(actual);

  vector<pair<ubicacion, Action>> candidatas;

  if (c == 'C' || c == 'S' || c == 'U')
    candidatas.push_back({Alante, WALK});
  if (i == 'C' || i == 'S' || i == 'U')
    candidatas.push_back({Izquierda(actual), TURN_SL});
  if (d == 'C' || d == 'S' || d == 'U')
    candidatas.push_back({Derecha(actual), TURN_SR});

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

  ubicacion destino = Alante;
  if (accion == TURN_SL)
    destino = Izquierda(actual);
  else if (accion == TURN_SR)
    destino = Derecha(actual);
  mapa_visitado[{destino.f, destino.c}]++;

  last_action = accion;
  return accion;
}

list<Action> AvanzaASaltosDeCaballo()
{
  list<Action> secuencia;
  secuencia.push_back(WALK);
  secuencia.push_back(WALK);
  secuencia.push_back(TURN_SR);
  secuencia.push_back(TURN_SR);
  secuencia.push_back(WALK);
  return secuencia;
}

// FUNCIONES NECESARIAS PARA B_Anchura

EstadoT NextCasillaTecnico(const EstadoT &st)
{
  EstadoT siguiente = st;
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

bool CasillaAccesibleTecnico(const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  EstadoT next = NextCasillaTecnico(st);
  bool check1 = false, check2 = false, check3 = false;
  check1 = terreno[next.site.f][next.site.c] != 'P' and terreno[next.site.f][next.site.c] != 'M';
  check2 = terreno[next.site.f][next.site.c] != 'B' or (terreno[next.site.f][next.site.c] == 'B' and st.zapatillas);
  check3 = abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]) <= 1;
  return check1 and check2 and check3;
}

EstadoT applyT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  EstadoT next = st;
  switch (accion)
  {
  case WALK:
    if (CasillaAccesibleTecnico(st, terreno, altura))
    {
      next = NextCasillaTecnico(st);
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
/*
bool Find(const NodoT &st, const list<NodoT> &lista)
{
  auto it = lista.begin();
  while (it != lista.end() and !((*it) == st))
  {
    it++;
  }
  return (it != lista.end());
}

list<Action> ComportamientoTecnico::B_Anchura_V2(const EstadoT &inicio, const EstadoT &final, const vector<vector<unsigned char>> &terreno, vector<vector<unsigned char>> &altura){
  NodoT current_node;
  list<NodoT> frontier;
  set<NodoT> explored;
  list<Action> path;

  current_node.estado = inicio;
  frontier.push_back(current_node);
  bool SolutionFound = (current_node.estado.site.f == final.site.f && current_node.estado.site.c == final.site.c);

  while (!SolutionFound and !frontier.empty()){
      frontier.pop_front();
      explored.insert(current_node);

      // Compruebo si estoy en una casilla que da las zapatillas
      if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D'){
          current_node.estado.zapatillas = true;
      }

      // Genero el hijo resultante de aplicar la acción WALK
      NodoT child_Walk = current_node;
      child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);
      if (child_Walk.estado.site.f == final.site.f and child_Walk.estado.site.c == final.site.c){
          // El hijo generado es solucion
          child_Walk.secuencia.push_back(WALK);
          current_node = child_Walk;
          SolutionFound = true;
      }
      else if (explored.find(child_Walk) == explored.end()){
          // Se mete en la lista de frontier después de añadir a secuencia la acción
          child_Walk.secuencia.push_back(WALK);
          frontier.push_back(child_Walk);
      }

      if (!SolutionFound){
          // El hijo resultante de aplicar la accion TURN_SR
          NodoT child_TurnSR = current_node;
          child_TurnSR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
          if (explored.find(child_TurnSR) == explored.end()){
              child_TurnSR.secuencia.push_back(TURN_SR);
              frontier.push_back(child_TurnSR);
          }

          // El hijo resultante de aplicar la accion TURN_SL
          NodoT child_TurnSL = current_node;
          child_TurnSL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
          if (explored.find(child_TurnSL) == explored.end()){
              child_TurnSL.secuencia.push_back(TURN_SL);
              frontier.push_back(child_TurnSL);
          }
      }

      // Paso a evaluar el siguiente nodo en la lista "frontier"
      if (!SolutionFound and !frontier.empty()){
          current_node = frontier.front();
          while(explored.find(current_node) != explored.end() and !frontier.empty()){
            frontier.pop_front();
            current_node = frontier.front();
          }
          //SolutionFound = (current_node.estado.site.f == final.site.f and current_node.estado.site.c == final.site.c);
      }
  }
  // Devuelvo el camino encontrado.

  if (SolutionFound)
    path = current_node.secuencia;

  return path;
}


list<Action> ComportamientoTecnico::B_Anchura(const EstadoT &inicio, const EstadoT &final, const vector<vector<unsigned char>> &terreno, vector<vector<unsigned char>> &altura){
  NodoT current_node;
  list<NodoT> frontier;
  list<NodoT> explored;
  list<Action> path;

  current_node.estado = inicio;
  frontier.push_back(current_node);
  bool SolutionFound = (current_node.estado.site.f == final.site.f && current_node.estado.site.c == final.site.c);

  while (!SolutionFound and !frontier.empty()){
      frontier.pop_front();
      explored.push_back(current_node);

      // Compruebo si estoy en una casilla que da las zapatillas
      if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D'){
          current_node.estado.zapatillas = true;
      }

      // Genero el hijo resultante de aplicar la acción WALK
      NodoT child_Walk = current_node;
      child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);
      if (child_Walk.estado.site.f == final.site.f and child_Walk.estado.site.c == final.site.c){
          // El hijo generado es solucion
          child_Walk.secuencia.push_back(WALK);
          current_node = child_Walk;
          SolutionFound = true;
      }
      else if (!Find(child_Walk, frontier) and !Find(child_Walk, explored)){
          // Se mete en la lista de frontier después de añadir a secuencia la acción
          child_Walk.secuencia.push_back(WALK);
          frontier.push_back(child_Walk);
      }

      if (!SolutionFound){
          // El hijo resultante de aplicar la accion TURN_SR
          NodoT child_TurnSR = current_node;
          child_TurnSR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
          if (!Find(child_TurnSR, frontier) and !Find(child_TurnSR, explored)){
              child_TurnSR.secuencia.push_back(TURN_SR);
              frontier.push_back(child_TurnSR);
          }

          // El hijo resultante de aplicar la accion TURN_SL
          NodoT child_TurnSL = current_node;
          child_TurnSL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
          if (!Find(child_TurnSL, frontier) and !Find(child_TurnSL, explored)){
              child_TurnSL.secuencia.push_back(TURN_SL);
              frontier.push_back(child_TurnSL);
          }
      }

      // Paso a evaluar el siguiente nodo en la lista "frontier"
      if (!SolutionFound and !frontier.empty()){
          current_node = frontier.front();
          SolutionFound = (current_node.estado.site.f == final.site.f and current_node.estado.site.c == final.site.c);
      }
  }
  // Devuelvo el camino encontrado.

  if (SolutionFound)
    path = current_node.secuencia;

  return path;
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_E(Sensores sensores)
{
  Action accion = IDLE;

  if (!hayPlan)
  {
    EstadoT inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;
    plan = B_Anchura_V2(inicio, fin, mapaResultado, mapaCotas);
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

*/

int ComportamientoTecnico::modifica_altura(Action accion, unsigned char terreno, int altura_origen, int altura_destino)
{
  if (accion == WALK)
  {
    if (terreno == 'C' || terreno == 'U' || terreno == 'P' || terreno == 'M')
    {
      return 0;
    }

    if (altura_destino > altura_origen)
    {
      return 5;
    }
    else if (altura_destino < altura_origen)
    {
      return -2;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

int ComportamientoTecnico::CalculoEnergia(Action accion, unsigned char terreno)
{
  if (accion == WALK)
  {
    switch (terreno)
    {
    case 'A':
      return 60;
    case 'H':
      return 6;
    case 'S':
      return 3;
    default:
      return 1;
    }
  }
  else if (accion == TURN_SL || accion == TURN_SR)
  {
    switch (terreno)
    {
    case 'A':
      return 5;
    case 'H':
      return 2;
    case 'S':
      return 1;
    default:
      return 1;
    }
  }
}

list<Action> ComportamientoTecnico::BusquedaEstrella(const EstadoT &inicio, const EstadoT &final, const vector<vector<unsigned char>> &terreno, vector<vector<unsigned char>> &altura)
{
  NodoT nodo_actual;

  priority_queue<NodoT, vector<NodoT>, greater<NodoT>> frontier; // ordenamos de forma ascendente
  // set<NodoT> explored;
  map<EstadoT, int> explored;

  nodo_actual.estado = inicio;
  nodo_actual.g = 0;
  nodo_actual.h = 0;

  if (inicio.site.f == final.site.f && inicio.site.c == final.site.c)
  {
    return list<Action>();
  }

  if (terreno[nodo_actual.estado.site.f][nodo_actual.estado.site.c] == 'D')
  {
    nodo_actual.estado.zapatillas = true;
  }

  frontier.push(nodo_actual);

  Action posibles_acciones[] = {WALK, TURN_SL, TURN_SR};
  while (!frontier.empty())
  {
    nodo_actual = frontier.top();
    frontier.pop();

    // objetivo encontrado
    if (nodo_actual.estado.site.f == final.site.f and nodo_actual.estado.site.c == final.site.c)
    {
      return nodo_actual.secuencia;
    }

    // explored.insert(nodo_actual);
    if (explored.count(nodo_actual.estado) and explored[nodo_actual.estado] <= nodo_actual.g)
    {
      continue;
    }
    explored[nodo_actual.estado] = nodo_actual.g;

    for (Action accion : posibles_acciones)
    {
      NodoT hijo = nodo_actual;

      hijo.estado = applyT(accion, nodo_actual.estado, terreno, altura);

      // comprobamos que no se choca contra un muro, para evitar un bucle infinito

      if (accion == WALK and hijo.estado.site.f == nodo_actual.estado.site.f and hijo.estado.site.c == nodo_actual.estado.site.c)
      {
        continue; // descartamos al hijo porque el movimiento es ilegal, applyT ha dicho que por ahí no se puede
      }

      unsigned char terreno_origen = terreno[nodo_actual.estado.site.f][nodo_actual.estado.site.c];
      unsigned char terreno_destino = terreno[hijo.estado.site.f][hijo.estado.site.c];
      int alt_origen = altura[nodo_actual.estado.site.f][nodo_actual.estado.site.c];
      int alt_destino = altura[hijo.estado.site.f][hijo.estado.site.c];

      int coste = CalculoEnergia(accion, terreno_origen);
      int mod_altura = modifica_altura(accion, terreno_destino, alt_origen, alt_destino);

      hijo.g = nodo_actual.g + coste + mod_altura;

      // calculo heuristica-> NOTA: No usamos la distancia de Pitágoras porque un movimiento en diagonal le da el valor 1.41, cuando realmente cuesta 1 porque el simulador admite
      // andar en diagonal => usamos distancia de Chebyshev
      int d_f = abs(final.site.f - hijo.estado.site.f);
      int d_c = abs(final.site.c - hijo.estado.site.c);

      hijo.h = max(d_f, d_c);

      hijo.secuencia.push_back(accion);

      if (terreno[hijo.estado.site.f][hijo.estado.site.c] == 'D')
      {
        hijo.estado.zapatillas = true;
      }

      frontier.push(hijo);
    }
  }

  return list<Action>();
}

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores)
{
  Action accion = IDLE;
  // El comportamiento que tiene que tener es NO estorbar al ingeniero

  if (giros_forzados > 0)
  {
    giros_forzados--;
    accion = TURN_SL;
    last_action = accion;
    return accion;
  }
  if (sensores.agentes[2] == 'i')
  {
    giros_forzados = 2;
    accion = TURN_SL;
    last_action = accion;
    return accion;
  }

  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores)
{
  Action accion = IDLE;
  if (!hayPlan)
  {
    EstadoT inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;
    plan = BusquedaEstrella(inicio, fin, mapaResultado, mapaCotas);
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
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores)
{
  return IDLE;
}

Orientacion ComportamientoTecnico::calcularOrientacion(EstadoT desde, EstadoT hacia)
{
  int dif_f = hacia.site.f - desde.site.f;
  int dif_c = hacia.site.c - desde.site.c;

  if (dif_f == -1)
    return norte;
  if (dif_c == 1)
    return este;
  if (dif_f == 1)
    return sur;
  if (dif_c == -1)
    return oeste;

  return norte;
}

Action ComportamientoTecnico::girarHacia(Orientacion actual, Orientacion objetivo)
{
  int diferencia = (objetivo - actual + 8) % 8;

  if (diferencia <= 4)
    return TURN_SR;
  return TURN_SL;
}

Orientacion calcularCasillaObjetivo(int ft, int ct, int fi, int ci, Orientacion rumbo)
{
  if (fi != ft and ci != ct)
  { // caso casilla diagonal
    if ((fi < ft and ci > ct and rumbo == norte) || (fi > ft and ci > ct and rumbo == sur))
      return oeste;
    if ((fi > ft and ci < ct and rumbo == oeste) || (fi > ft and ci > ct and rumbo == este))
      return norte;
    if ((fi < ft and ci < ct and rumbo == oeste) || (fi < ft and ci > ct and rumbo == este))
      return sur;
    if ((fi < ft and ci < ct and rumbo == norte) || (fi > ft and ci < ct and rumbo == sur))
      return este;
  }
  else // caso casilla recta (no diagonal)
  {
    if (fi == ft and ci > ct and rumbo == este)
      return oeste;
    if (fi == ft and ci < ct and rumbo == oeste)
      return este;
    if (fi < ft and ci == ct and rumbo == norte)
      return sur;
    if (fi > ft and ci == ct and rumbo == sur)
      return norte;
  }

  return norte;
}
/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores)
{
  Action accion = IDLE;

  switch (estado_instalacion)
  {
  case IR_A_BELKANITA:
  {
    if (sensores.posF == sensores.BelPosF && sensores.posC == sensores.BelPosC)
    {
      llegaBelk = true;
      estado_instalacion = ESPERAR_LLAMADA;
      hayPlan = false;
      plan.clear();
      tuberias = 2; 
      break;
    }

    if (tuberias == 0)
    {
      int distancia_belk = abs(sensores.posF - sensores.BelPosF) + abs(sensores.posC - sensores.BelPosC);
      if (distancia_belk <= 2)
      {
        cout << "Técnico en posición de espera. Esperando llamada..." << endl;
        tuberias = 1;
        estado_instalacion = ESPERAR_LLAMADA;
        hayPlan = false;
        plan.clear();
        accion = IDLE;
        break;
      }
    }
    if (!hayPlan)
    {
      cout << "Buscando el plan a la belkanita..." << endl;
      EstadoT inicio, fin;
      inicio.site.f = sensores.posF;
      inicio.site.c = sensores.posC;
      inicio.site.brujula = sensores.rumbo;
      inicio.zapatillas = tiene_zapatillas;
      fin.site.f = sensores.BelPosF;
      fin.site.c = sensores.BelPosC;

      vector<vector<unsigned char>> mapaTemporal = mapaResultado;

      if (sensores.agentes[2] == 'i')
      {
        ubicacion miPos = {sensores.posF, sensores.posC, sensores.rumbo};
        ubicacion ingPos = Delante(miPos);

        if (ingPos.f >= 0 && ingPos.f < mapaTemporal.size() && ingPos.c >= 0 && ingPos.c < mapaTemporal[0].size())
        {
          mapaTemporal[ingPos.f][ingPos.c] = 'M';
        }
      }
      plan = BusquedaEstrella(inicio, fin, mapaTemporal, mapaCotas);
      VisualizaPlan(inicio.site, plan);
      hayPlan = !plan.empty();
    }
    else
    {
      accion = plan.front();
      if (accion == WALK && (sensores.agentes[2] == 'i' || sensores.choque))
      {
        hayPlan = false;
        plan.clear();
        accion = IDLE;
      }
      else
      {
        plan.pop_front();
        if (plan.empty()) hayPlan = false;
      }
    }
    break;
  }
  case ACERCARSE:
  {
    
    
      if (abs(sensores.posF - sensores.GotoF) + abs(sensores.posC - sensores.GotoC) == 1)
      {
        if (!sensores.enfrente)
        {
          estado_instalacion = ORIENTARSE;
        }
        else
        {
          estado_instalacion = ESPERAR_LLAMADA;
          accion = INSTALL;
        }
        hayPlan = false;
        plan.clear();
      }
      else
      {
        if (!hayPlan)
        {

          cout << "NO hay plan" << endl;
          EstadoT inicio = {sensores.posF, sensores.posC, sensores.rumbo};
          EstadoT fin = {sensores.GotoF, sensores.GotoC, norte};
          Orientacion obj = calcularCasillaObjetivo(sensores.posF, sensores.posC, sensores.GotoF, sensores.GotoC, sensores.rumbo);

          switch (obj)
          {
          case norte:
          {
            fin.site.f--;
            break;
          }
          case sur:
          {
            fin.site.f++;
            break;
          }
          case este:
          {
            fin.site.c++;
            break;
          }
          case oeste:
          {
            fin.site.c--;
            break;
          }
          }
          if (fin.site.f >= 0 && fin.site.f < mapaResultado.size() &&
              fin.site.c >= 0 && fin.site.c < mapaResultado[0].size())
          {
            cout << fin.site.f << fin.site.c << inicio.site.f << inicio.site.c << endl;
            plan = BusquedaEstrella(inicio, fin, mapaResultado, mapaCotas);
            hayPlan = !plan.empty();
          }
          else
          {
            hayPlan = false;
          }
        }
        else if (hayPlan and !plan.empty())
        {
          accion = plan.front();
          plan.pop_front();
          if (plan.empty())
            hayPlan = false;
        }
      }
    

    break;
  }

  case ESPERAR_LLAMADA:
  {
    if (sensores.enfrente)
    {
      accion = INSTALL;
    }
    if (sensores.venpaca)
    {
      if (tuberias <= 1)
      {
        // Todavía tenemos que pisar la Belkanita
        estado_instalacion = IR_A_BELKANITA;
      }
      else
      {
        // Ya instalamos la tubería 1, ahora perseguimos al ingeniero
        estado_instalacion = ACERCARSE;
      }
    }
    break;
  }

  case ORIENTARSE:
  {
    EstadoT pos_actual = {sensores.posF, sensores.posC, norte};
    EstadoT pos_objetivo = {sensores.GotoF, sensores.GotoC, norte};
    Orientacion necesaria = calcularOrientacion(pos_actual, pos_objetivo);

    if (sensores.rumbo != necesaria)
    {
      accion = girarHacia(sensores.rumbo, necesaria);
    }
    else
    {
      accion = INSTALL;
      estado_instalacion = ESPERAR_LLAMADA;
      // estado_instalacion = INSTALAR_T;
    }
    break;
  }
  }

  cout << "ESTADO_T: " << estado_instalacion << endl;
  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores)
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
void ComportamientoTecnico::ActualizarMapa(Sensores sensores)
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
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1)
    return false;
  return true;
}

ubicacion ComportamientoTecnico::Izquierda(const ubicacion &actual) const
{
  // vamos a aprovechar la función Delante que tenemos, voy a calcular el rumbo
  // Nuevo, sabiendo que girar 90º es restarle 2 (en mod 8, porque se divide en giros de 45º)
  // El casteo cambia 0 por norte, 1 noroeste...
  ubicacion izq = {actual.f, actual.c, actual.brujula};
  izq.brujula = static_cast<Orientacion>((actual.brujula + 6) % 8); // +6 = -2 mod 8
  return Delante(izq);
}

ubicacion ComportamientoTecnico::Derecha(const ubicacion &actual) const
{
  ubicacion der = {actual.f, actual.c, actual.brujula};
  der.brujula = static_cast<Orientacion>((actual.brujula + 2) % 8);
  return Delante(der);
}
/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const
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
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
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
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
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
