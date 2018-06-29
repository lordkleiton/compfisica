import processing.serial.*;                                                                         //importação da biblioteca serial
Serial myPort;                                                                                      //objeto da porta serial

//variaveis
String val;                                                                                         //armazena o que o arduino envia
JSONArray json1, auxjson1, json2, auxjson2;                                                         //vários json que uso pois sou medroso na hora de mexer em arquivos
boolean firstContact = false;                                                                       //faz a checagem da comunicação entre arduino e processing

//inicializa tudo
void setup() {
  size(200, 200);                                                                                   //processing precisa de uma janela sempre aberta
  myPort = new Serial(this, Serial.list()[0], 9600);                                                //instancia a porta serial
  myPort.bufferUntil('\n');                                                                         //lê a porta serial sempre até a quebra de linha
  println(ler());                                                                                   //importante pra debug
}

//função que desenha na tela, mas aqui ela nunca chega a ser utilizada
void draw() {
  
}

//aqui a magia acontece
void serialEvent( Serial myPort) {
  val = myPort.readStringUntil('\n');                                                               //como disse no setup, sempre lê o que recebe até a quebra de linha

  if (val != null) {                                                                                //se o recebido não for nulo

    val = trim(val);                                                                                //corta as informações desnecessárias da mensagem recebida
    println("\n");                                                                                  //questão de estética

    if (firstContact == false) {                                                                    //se achar o A que o arduino sempre manda, estabelece a conexão com o arduino
      if (val.equals("A")) {
        myPort.clear();
        firstContact = true;
        myPort.write("A");
        println("contact");
      }
      } else {                                                                                      //se tiver dados que não sejam o handshake com o arduino
      String data = year() + "-" + month() + "-" + day() + ", " + hour() + ":" + minute() + ":" + second();  //define a data e hora
      println(val + "\n" + data);                                                                   //imprime tudo no console, seja cadastrado ou não

      JSONArray autorizado = ler();                                                                 //varíavel que recebe as informações lidas do arquivo de autorizados
      JSONObject atual;                                                                             //auxiliar pras informações lidas
      int contador = 0;                                                                             //ajuda na iteração
      String name = "";                                                                             //sou obrigado a inicializar sempre

      for (int i = 0; i < autorizado.size(); i++) {                                                 //percorre o arquivo inteiro de autorizados
        atual = autorizado.getJSONObject(i);                                                        //atual sempre recebe o valor de um dos membros de autorizados

        if ((val.equals(trim(atual.getString("rfid")))) || (val.equals(trim(atual.getString("password")))))  //e compara rfid e senha com o que recebe do arduino
        {         
          name = atual.getString("name");
          contador++;
          break;                                                                                    //se der certo, para o for
        }
      }

      if (contador == 1)                                                                            //se o contador for 1
      {                           
        myPort.write('0');                                                                          //envia o código 0, que significa autorizado
        println("autorizado");
        escreve(val, name, data);                                                                   //escreve no json de log o que aconteceu
        println(name);                                                                              //e printa no log do processing 
      } else {                                                                                      //caso não
        myPort.write('1');                                                                          //envia 1, que significa falha
        println("nao cadastrado");                                                                  //printa no log do processing
      }
      
      myPort.write("A");                                                                            //por fim, avisa ao arduino que tá esperando mais dados serem enviados
    }
  }
}

//escreve o log de acessos em disco
void escreve(String rfid, String name, String data) {
  json1 = loadJSONArray("log.json");
  auxjson1 = json1;

  JSONObject log = new JSONObject();

  log.setInt("id", auxjson1.size());
  log.setString("name", name);
  log.setString("rfid", rfid);
  log.setString("date", data);

  auxjson1.setJSONObject(auxjson1.size(), log);

  saveJSONArray(auxjson1, "data/log.json");
}

//le a lista de rfids autorizados
JSONArray ler() {
  json2 = loadJSONArray("authorized.json");
  auxjson2 = json2;

  return auxjson2;
}
