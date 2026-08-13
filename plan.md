Sí. Para ft_irc, si vuestro objetivo no es solo “repartir archivos”, sino que los tres aprendáis sockets, protocolo IRC, concurrencia, parsing y C++, lo mejor es dividirlo por responsabilidades principales, pero hacer que cada persona tenga una parte de casi todas las capas.

Asumiendo el ft_irc típico de 42 (servidor IRC en C++98), estos serían los componentes.

1. Componentes básicos

Podéis pensar el servidor como estas piezas:

                    ┌─────────────────────┐
                    │      IRC Client     │
                    │  (netcat / irssi)   │
                    └──────────┬──────────┘
                               │ TCP
                               ▼
                    ┌─────────────────────┐
                    │     TCP Server      │
                    │ socket/bind/listen  │
                    │ accept/select/poll  │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │  Client / Connection │
                    │  fd + buffer + nick  │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │       Parser        │
                    │  "PRIVMSG #42 :Hi"  │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │ Command Dispatcher  │
                    │ PASS NICK USER JOIN  │
                    │ PRIVMSG PART QUIT...│
                    └──────────┬──────────┘
                               │
             ┌─────────────────┴─────────────────┐
             ▼                                   ▼
     ┌────────────────┐                 ┌────────────────┐
     │     Client     │                 │     Channel    │
     │ nick/user/etc. │                 │ members/modes  │
     └────────────────┘                 └────────────────┘

Los componentes que yo consideraría fundamentales son:

Servidor TCP
socket()
bind()
listen()
accept()
poll()/select()
recepción y envío de datos.
Gestión de conexiones
Clientes conectados.
File descriptors.
Buffers de entrada.
Desconexiones.
Mensajes parciales.
Parser IRC
Separar comandos.
Argumentos.
Parámetros que empiezan por :.
CRLF (\r\n).
Manejo de mensajes incompletos.
Usuarios/Clients
Nick.
Username.
Password/autenticación.
Estado de registro.
Canales en los que participa.
Channels
Nombre del canal.
Lista de usuarios.
Operadores.
Invitados, según los modos que implementéis.
Modos del canal.
Command handlers
PASS
NICK
USER
JOIN
PART
PRIVMSG
KICK
INVITE
TOPIC
MODE
QUIT
Respuestas y errores IRC
Por ejemplo, ERR_NOSUCHNICK, ERR_NOSUCHCHANNEL, etc.
Formateo correcto de respuestas.
Gestión de modos
Modos de usuario/canal requeridos por el subject.
+i, +t, +k, +o, +l, etc., dependiendo de vuestra implementación.
Broadcasting
Enviar un mensaje a todos los miembros de un canal.
Evitar enviárselo al propio emisor cuando corresponda.
Mensajes de JOIN/PART/QUIT/etc.
2. Cómo lo dividiría entre tres

Yo evitaría algo como:

Persona A → sockets
Persona B → comandos
Persona C → canales

porque la persona de sockets acabaría aprendiendo mucho de redes y poco del protocolo, mientras que la de comandos acabaría escribiendo simplemente muchos if.

Mejor:

Persona 1 — Networking + conexiones

Responsabilidad principal:

Server
 ├── socket
 ├── bind
 ├── listen
 ├── poll/select
 ├── accept
 ├── recv
 └── send

Pero también debería implementar algunos comandos.

Por ejemplo:

PASS
NICK
USER
QUIT

Así aprende:

sockets TCP
file descriptors
poll()
clientes simultáneos
buffers
parsing
protocolo IRC
estados de un cliente
respuestas IRC
Persona 2 — Parser + protocolo + comandos

Responsabilidad principal:

Raw data
   ↓
Buffer
   ↓
IRC message
   ↓
Parser
   ↓
Command
   ↓
Handler

Implementaría, por ejemplo:

JOIN
PART
PRIVMSG
NOTICE
TOPIC

Y se encargaría especialmente de:

parsing
validación de argumentos
errores
formato de mensajes
numerics IRC
dispatcher de comandos

Pero esta persona también tendría que tocar sockets, por ejemplo colaborando en el tratamiento del buffer recibido y en send().

Persona 3 — Channels + estados + modos

Responsabilidad principal:

Channel
 ├── members
 ├── operators
 ├── topic
 ├── modes
 └── permissions

Implementaría, por ejemplo:

KICK
INVITE
MODE
parte de JOIN
parte de PRIVMSG

Y trabajaría en:

estructura de canales
relaciones Client ↔ Channel
permisos
operadores
modos
broadcast
validación de acciones

Pero también debería participar en networking y parsing, por ejemplo implementando y probando el flujo completo de MODE desde el socket hasta la respuesta.

3. La clave: compartir ciertas piezas

Hay componentes que no deberían pertenecer exclusivamente a una persona.

Por ejemplo:

Client

Los tres deberían entenderlo.

class Client
{
    int         _fd;
    std::string _nickname;
    std::string _username;
    std::string _buffer;
    bool        _registered;

    // ...
};

Porque:

networking necesita el fd;
parser necesita el buffer;
comandos necesitan nick/user;
channels necesitan saber quién es el cliente.
Channel

También debería ser conocido por los tres:

class Channel
{
    std::string              _name;
    std::string              _topic;
    std::vector<Client *>    _clients;
    std::vector<Client *>    _operators;

    // ...
};
Server

Igualmente:

class Server
{
    int _socketFd;

    std::map<int, Client *>       _clients;
    std::map<std::string, Channel *> _channels;

    // ...
};

No conviene que una persona sea “la única que sabe cómo funciona Server”.

4. Reparto concreto de comandos

Una distribución bastante equilibrada podría ser:

Persona	Comandos principales	Área principal
A	PASS, NICK, USER, QUIT	Networking + Client
B	JOIN, PART, PRIVMSG, NOTICE	Parser + protocolo
C	KICK, INVITE, TOPIC, MODE	Channels + permisos

Pero hay una cosa importante:

JOIN y PRIVMSG deberían hacerse conjuntamente entre B y C.

Porque obligan a entender prácticamente todo:

Cliente
   ↓
socket
   ↓
recv()
   ↓
buffer
   ↓
parser
   ↓
JOIN
   ↓
buscar/crear Channel
   ↓
añadir Client
   ↓
broadcast
   ↓
send()

Ese flujo es probablemente uno de los mejores ejercicios para que los tres aprendáis el proyecto entero.

5. Cómo trabajar para que todos aprendan todo

Os recomendaría hacerlo en fases, no simplemente trabajar tres ramas independientes durante dos semanas.

Fase 1 — Los tres juntos

Implementad entre los tres:

socket
   ↓
bind
   ↓
listen
   ↓
poll
   ↓
accept
   ↓
client

Objetivo: conseguir que dos clientes puedan conectarse.

Aquí los tres tienen que entender el código.

Fase 2 — Los tres juntos

Implementad:

recv()
 ↓
buffer
 ↓
\r\n
 ↓
parse

Y probad:

PASS password\r\n
NICK pepe\r\n
USER pepe 0 * :Pepe\r\n

Los tres deberían poder explicar exactamente qué ocurre desde que llega el byte por el socket hasta que se crea el usuario.

Fase 3 — División

A partir de aquí sí:

              ┌─────────────┐
              │   COMMON    │
              │ Server      │
              │ Client      │
              │ Parser      │
              │ Responses   │
              └──────┬──────┘
                     │
       ┌─────────────┼─────────────┐
       ▼             ▼             ▼
      A              B             C
 Networking       Commands       Channels
 Client           IRC protocol   Modes
 QUIT             JOIN           MODE
 NICK             PRIVMSG        KICK
 USER             PART           INVITE
 PASS             TOPIC          ...
6. Rotación para aprender

Aquí está lo que creo que os puede venir mejor.

Cuando una persona termina una funcionalidad, otra tiene que revisarla y explicarla.

Por ejemplo:

Semana 1

A implementa NICK.

B revisa NICK.

C hace las pruebas.

Semana 2

B implementa JOIN.

C revisa JOIN.

A hace las pruebas.

Semana 3

C implementa MODE.

A revisa MODE.

B hace las pruebas.

De esta manera no ocurre:

"Ese código es de X, pregúntale a X."

Todos termináis conociendo todo.

7. Una arquitectura sencilla

Para un ft_irc intentaría no complicarlo demasiado:

src/
├── main.cpp
│
├── Server/
│   ├── Server.hpp
│   └── Server.cpp
│
├── Client/
│   ├── Client.hpp
│   └── Client.cpp
│
├── Channel/
│   ├── Channel.hpp
│   └── Channel.cpp
│
├── Parser/
│   ├── Parser.hpp
│   └── Parser.cpp
│
├── Commands/
│   ├── Command.hpp
│   ├── Pass.cpp
│   ├── Nick.cpp
│   ├── User.cpp
│   ├── Join.cpp
│   ├── Part.cpp
│   ├── Privmsg.cpp
│   ├── Kick.cpp
│   ├── Invite.cpp
│   ├── Topic.cpp
│   ├── Mode.cpp
│   └── Quit.cpp
│
└── Utils/
    ├── Utils.hpp
    └── Utils.cpp

Y conceptualmente:

                    Server
                      │
          ┌───────────┴───────────┐
          │                       │
       Clients                 Channels
          │                       │
          └──────────┬────────────┘
                     │
                  Commands
                     │
                   Parser
                     │
                  Network

No intentaría crear una arquitectura excesivamente abstracta para un proyecto de este tamaño. Que el flujo de un comando sea fácil de seguir vale más que tener 20 clases "elegantes".

8. Los flujos que deberíais conseguir entender los tres

Si al final cualquiera de vosotros puede coger un comando y explicar esto, vais muy bien:

NICK
recv
 ↓
parse
 ↓
"NICK"
 ↓
buscar Client
 ↓
validar nick
 ↓
cambiar nick
 ↓
notificar
 ↓
send
JOIN
recv
 ↓
parse
 ↓
JOIN #42
 ↓
buscar Channel
 ↓
crear si no existe
 ↓
comprobar permisos
 ↓
añadir Client
 ↓
broadcast JOIN
 ↓
send
PRIVMSG
recv
 ↓
parse
 ↓
PRIVMSG #42 :hola
 ↓
buscar #42
 ↓
buscar miembros
 ↓
construir mensaje
 ↓
broadcast
 ↓
send
MODE
recv
 ↓
parse
 ↓
MODE #42 +i
 ↓
buscar Channel
 ↓
comprobar operador
 ↓
modificar modo
 ↓
notificar
 ↓
send

Estos cuatro flujos prácticamente os obligan a entender todo el proyecto.

Mi recomendación final

Para tres personas, haría esta división:

Persona A

Networking
Server
gestión de Client
PASS, NICK, USER, QUIT

Persona B

Parser
dispatcher
respuestas IRC
JOIN, PART, PRIVMSG, NOTICE

Persona C

Channel
permisos
modos
KICK, INVITE, TOPIC, MODE

Pero con una regla:

Nadie puede hacer una funcionalidad sin que otra persona la revise y pueda explicarla.

Y las partes críticas (Server, Client, Parser, JOIN, PRIVMSG y MODE) las trabajaría entre los tres, al menos durante la primera implementación.

Si queréis, también puedo proponeros un orden exacto de implementación del ft_irc, desde socket() hasta tener todos los comandos funcionando, indicando qué debería hacer cada persona en cada etapa.