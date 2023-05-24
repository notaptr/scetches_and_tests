#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT            17333
#define ADDR            INADDR_ANY
#define RECV_BUFF_SIZE  4096
#define CLIENTS         10

#define CONNECTION_OK   "Подключение установлено"
#define CLIENT_DOWN     "Клиент недоступен"
#define INVALID_PACKET  "Недопустимый пакет данных"


//----- GLOBALS ------

struct sockaddr_in server;
int udp_socket;
char *buff;

struct peer {
	int	peer_id;					//идентификатор отправителя, если 0 - не активен
	struct sockaddr peer_addr;		//адрес отправителя, заполняется при получении пакета
	socklen_t soclen;				//размер структуры
	int msg_id;						//номер последнего подтверждённого сообщения
	char name[100];                 //имя клиента
};

struct peer clients[CLIENTS];

//-------- PROGRAM --------

int init_socket() {
	int rez;
	
    server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
	
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	
	rez = bind(udp_socket, (struct sockaddr*)&server, sizeof(server));
	
	return rez;
}

int route_message(int r_id, char* msg, int ln) {

		struct peer* recp = &clients[ r_id ];
		int rez;
		
		for (int ii = 0; ii < 5; ii++) {

			rez = sendto(udp_socket, msg, ln, 0, (struct sockaddr*)&recp->peer_addr, recp->soclen);
			
			if (rez != -1) { break; }
			
			sleep(1);
			
		} //for ii
        
		if (rez == -1) {
			recp->peer_id = 0;
			printf("sendto() error! (%d, %s)\n", errno, strerror(errno));
		}

        return rez;
}

void start_server() {

	struct sockaddr_in client;
	socklen_t clen;
	int s_id, r_id, m_id, rez, len;
   
	buff = malloc(RECV_BUFF_SIZE);
	
	int work = 1;
	while(work) {
		
		len = recvfrom(udp_socket, buff, RECV_BUFF_SIZE, 0, (struct sockaddr*)&client, &clen);
		
		if (len > 12) {
			m_id = ((int*)buff)[0];
			s_id = ((int*)buff)[1];
			r_id = ((int*)buff)[2];
			
			if ( (s_id + r_id) > CLIENTS+1 ) {
				printf("Отправитель или Получатель вне диапазона\n");
				continue;
			}
			
			
			buff[len] = '\x0';
			
			printf("Отправитель - %d, Получатель - %d, сообщение: %s\n", s_id, r_id, buff + 12);
			
			int pp = s_id - 1; //индекс в массиве
			
			clients[ pp ].peer_id = s_id;
			memcpy( (void *) &clients[ pp ].peer_addr, (void *) &client, sizeof(client));
			clients[ pp ].soclen = clen;
			
			//--- подтверждение получения ---
			for (int ii = 0; ii < 5; ii++) {
				rez = sendto(udp_socket, buff, 4, 0, (struct sockaddr*)&client, clen);
				if ( rez == -1) {
					sleep(1);
					continue;
				}
				//clients[ pp ].peer_id = 0;
				break;
			}
				
			//Если сообщение старое, то ничего не пересылаем
			if (m_id == clients[ pp ].msg_id) { continue; }
			
			clients[ pp ].msg_id = m_id;
			
			// обращение к серверу, регистрация
			if (r_id == 0) {
				sendto(udp_socket, CONNECTION_OK, sizeof(CONNECTION_OK), 0, (struct sockaddr*)&client, clen);
				continue;
			} 
			
			// сообщаем, что адресат не активен
			if (clients[ r_id - 1 ].peer_id == 0) {
				sendto(udp_socket, CLIENT_DOWN, sizeof(CLIENT_DOWN), 0, (struct sockaddr*)&client, clen);
				continue;
			}			
			
			route_message(r_id, buff, len);
			
		} else {
			
			printf("invalid packet lenght: %d\n", len);
			
		} //if 
		      
	} //while

}

int main(int argc, char **argv)
{
	int rez;
	rez = init_socket();
	
	if (rez == -1) {
		printf("bind error! (%d, %s)\n", errno, strerror(errno));
		close(udp_socket);
		return 1;
	}
	
	memset( (void*) clients, 0, sizeof(clients) );
	start_server();
	
	close(udp_socket);
	free(buff);
	return 0;
}

