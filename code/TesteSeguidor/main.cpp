#include "mbed.h"
#define tempo_carga 20 //us
//-------------------------------------------//--------------------------------------------//
/* Constantes genéricas do código, usar para troca de cor da pista. */
#define PRETO 1
#define BRANCO 0
#define LINHA PRETO
#define FORA BRANCO
#define yCG2 49 //yCG^2
float lRodas = 8.5;
LocalFileSystem pasta("local");
Ticker flipper;
//-------------------------------------------//--------------------------------------------//
/* Pinagem dos sensores na placa. */
DigitalInOut s9(p5); // Sensor de chegada
DigitalInOut s1(p6);
DigitalInOut s2(p7);
DigitalInOut s3(p8);
DigitalInOut s4(p9);
DigitalInOut s5(p10);
DigitalInOut s6(p11);
DigitalInOut s7(p12);
DigitalInOut s8(p13);
//-------------------------------------------//--------------------------------------------//
/* Pinagem dos motores na placa. */
PwmOut motor_dirTras(p23); //motor da direita anti-horario
PwmOut motor_dirFrente(p24);  //motor da direita horario
PwmOut motor_esqFrente(p22); //motor da esquerda anti-horario
PwmOut motor_esqTras(p21);  //motor da esquerda horario

//-------------------------------------------//--------------------------------------------//
// Margens de segurança do Sensor semi-analógico. 
float margem_preto = 0.9;
float margem_branco = 3;
int noLeituras = 0;
//-------------------------------------------//--------------------------------------------//
/* Definição de Debug */
#define Debug 1
#if Debug == 1
    Serial pc (USBTX, USBRX);
#endif
//-------------------------------------------//--------------------------------------------//
/* Definição de Variáveis de Estado do sistema */
float calibracao_preto[9], calibracao_branco[9];
float kP, kI, kD, Vmax;
float tVOLTA;
float  P=0;
float  I=0;
float  D=0;
float  controlador=0;
float  resposta=0;
float  ultima_leitura=0;
bool   stop=false;
//-------------------------------------------//--------------------------------------------//
float min (float x, float y){
    if (x > y)
        return y;
    return x;
    }
float max (float x, float y){
    if (x > y)
        return x;
    return y;
    }
    
void girar_motores(){
    motor_dirFrente = 0.1;
    motor_dirTras = 0;
    motor_esqFrente = 0;
    motor_esqTras = 0.1;
    }
void parar_motores(){
    motor_dirFrente = 0.0;
    motor_dirTras = 0.0;
    motor_esqFrente = 0.0;
    motor_esqTras = 0.0;
    return;
}
void calibrar(){
// Calibração dos sensores
// Devemos garantir que um dos sensores está exatamente acima da linha branca / preta
    Timer tempo, giro;
    // Guarda as variáveis de tempo de calibração de cada sensor
    int sensores_calibrar[9] = {0,0,0,0,0,0,0,0,0};
    int i, max_min[9][2] = {{-1000,4000000},{-1000,4000000},{-1000,4000000},{-1000,4000000},{-1000,4000000},{-1000,4000000},{-1000,4000000},{-1000,4000000},{-1000,4000000}};//{max,min}

//Descarga
//É feita a descarga de cada um dos sensores separadamente
    giro.reset();
    girar_motores();
    giro.start();
    while (giro < 1.4){

// Carga dos sensores
        s1.output();
        s1 = 1;
        s2.output();
        s2 = 1;
        s3.output();
        s3 = 1;
        s4.output();
        s4 = 1;
        s5.output();
        s5 = 1;
        s6.output();
        s6 = 1;
        s7.output();
        s7 = 1;
        s8.output();
        s8 = 1;
    //  s9.output();
    //  s9 = 1;
        wait_us(tempo_carga);

        tempo.reset();
        s1.input();
        tempo.start();
        while(s1.read()==1);
        tempo.stop();
        sensores_calibrar[0]=tempo.read_us();
    
        tempo.reset();
        s2.input();
        tempo.start();
        while(s2.read()==1);
        tempo.stop();
        sensores_calibrar[1]=tempo.read_us();
    
        tempo.reset();
        s3.input();
        tempo.start();
        while(s3.read()==1);
        tempo.stop();
        sensores_calibrar[2]=tempo.read_us();
    
        tempo.reset();
        s4.input();
        tempo.start();
        while(s4.read()==1);
        tempo.stop();
        sensores_calibrar[3]=tempo.read_us();
    
        tempo.reset();
        s5.input();
        tempo.start();
        while(s5.read()==1);
        tempo.stop();
        sensores_calibrar[4]=tempo.read_us();
    
        tempo.reset();
        s6.input();
        tempo.start();
        while(s6.read()==1);
        tempo.stop();
        sensores_calibrar[5]=tempo.read_us();
    
        tempo.reset();
        s7.input();
        tempo.start();
        while(s7.read()==1);
        tempo.stop();
        sensores_calibrar[6]=tempo.read_us();
    
        tempo.reset();
        s8.input();
        tempo.start();
        while(s8.read()==1);
        tempo.stop();
        sensores_calibrar[7]=tempo.read_us();
    
//    tempo.reset();
//    s9.input();
//    tempo.start();
//    while(s9.read()==1);
//    tempo.stop();
//    sensores_calibrar[8]=tempo.read_us();
        for(i=0;i<8;i++){
            if(sensores_calibrar[i]<max_min[i][1] && sensores_calibrar[i] > 0)
                max_min[i][1]=sensores_calibrar[i];
            if(sensores_calibrar[i]>max_min[i][0])
                max_min[i][0] = sensores_calibrar[i];
        }
    noLeituras++;
    }
    parar_motores();
    for(i=0;i<8;i++){
        calibracao_preto[i] = max_min[i][0] * margem_preto;
        calibracao_branco[i] = max_min[i][1] * margem_branco;
        calibracao_branco[i] = 90;
        }
    // Lembrando, maximo eh PRETO e minimo eh BRANCO.
}
float transposicao (float t, float calBranco, float calPreto){
    if (t < calBranco)
        return BRANCO;
    if (t > calPreto)
        return PRETO;
    return (t - calBranco)/(calPreto-calBranco);    
}
int dist_sensores(float *a,int size){
	int i,toggle, toggleState,distance;
	toggle = 0;
	distance = 0;
	for (i = 0; i < size; i++){
		if (!toggle && !a[i]){
			continue;
		}else if (!toggle && a[i]){
			toggle = 1;
		}else if (toggle ==1 && !a[i]){
			distance++;
			toggle = 2;
		}
		else if (toggle ==2 && !a[i])
			distance++;
		else if (toggle==2 && a[i])
			break;			
	}
	
	return distance;

	}
float ler_sensores(){
    Timer tempo;
    float valor_retorno=0.0;
   float pesos[9] = {-1,-0.73984,-0.45529,-0.15380,0.15380,0.45529,0.73984,1,1};
 	float testeScurva[8];
    float sensores_ativos[9] = {calibracao_preto[0],calibracao_preto[1],calibracao_preto[2],calibracao_preto[3],calibracao_preto[4],calibracao_preto[5],calibracao_preto[6],calibracao_preto[7],calibracao_preto[8]};
    float temp = 0;                    
    //Valor inicial Preto ou branco
    //Valores de leitura dos sensores variarão entre 0(branco) e 1(preto)
    int i=0;
    // Alimentação dos capacitores da placa de sensores
    s1.output();
    s1 = 1;
        s2.output();
    s2 = 1;
        s3.output();
    s3 = 1;
        s4.output();
    s4 = 1;
        s5.output();
    s5 = 1;
        s6.output();    
    s6 = 1;
        s7.output();
    s7 = 1;
        s8.output();
    s8 = 1;
    wait_us(tempo_carga);
    //Leitura dos valores
    //A leitura ocorre simultaneamente para todos os sensores
    s1.input();
    tempo.reset();
    tempo.start();
    while(tempo.read_us()<calibracao_preto[0] && s1);
    tempo.stop();
    sensores_ativos[0]=tempo.read_us(); //coloca os brancos p5

    s2.input();
    tempo.reset();
    tempo.start();
    while(tempo.read_us()<calibracao_preto[1] && s2);
    tempo.stop();
    sensores_ativos[1]=tempo.read_us(); //coloca os brancos p6

    s3.input();
    tempo.reset();
    tempo.start();
    while(tempo.read_us()<calibracao_preto[2] && s3);
    tempo.stop();
    sensores_ativos[2]=tempo.read_us(); //coloca os brancos p7

    s4.input();
    tempo.reset();
    tempo.start();
    while(tempo.read_us()<calibracao_preto[3] && s4);
    tempo.stop();
    sensores_ativos[3]=tempo.read_us(); //coloca os brancos p8

    s5.input();
    tempo.reset();
    tempo.start();
    while(tempo.read_us()<calibracao_preto[4] && s5);
    tempo.stop();
    sensores_ativos[4]=tempo.read_us(); //coloca os brancos p9

    s6.input();
    tempo.reset();
    tempo.start();
    while(tempo.read_us()<calibracao_preto[5] && s6);
    tempo.stop();
    sensores_ativos[5]=tempo.read_us(); //coloca os brancos p10

    s7.input();
    tempo.reset();
    tempo.start();
    while(tempo.read_us()<calibracao_preto[6] && s7);
    tempo.stop();
    sensores_ativos[6]=tempo.read_us(); //coloca os brancos p11

    s8.input();
    tempo.reset();
    tempo.start();
    while(tempo.read_us()<calibracao_preto[7] && s8);
    tempo.stop();
    sensores_ativos[7]=tempo.read_us(); //coloca os brancos p12

    for(i=0;i<8;i++){
        sensores_ativos[i]=transposicao(sensores_ativos[i], calibracao_branco[i], calibracao_preto[i]);
        }
    if (LINHA == BRANCO)
        for (i=0; i<8;i++){
            sensores_ativos[i]=1-sensores_ativos[i];
			}
	/*if (dist_sensores(sensores_ativos,8)>0){
		
		for(i=0;i<8;i++){
			testeScurva[i]=sensores_ativos[i]*pesos[i];
			testeScurva[i]=fabs(ultima_leitura-testeScurva[i]);
			if (testeScurva[i]>=1.5)
				sensores_ativos[i]=0;
	
			}
		}
	*/
	///for(i=0; i<8;i++){
	//		printf("%.5f ", sensores_ativos[i]);
		
	//}
	//printf("\n\r");

	for(i=0; i<8;i++){
        valor_retorno += sensores_ativos[i]*pesos[i];
        temp+=sensores_ativos[i];
        }
		
	if (temp ==0)// (fabs(valor_retorno/temp- ultima_leitura) > 1))
		return ultima_leitura;
    valor_retorno = valor_retorno / (temp);
	if ((valor_retorno-ultima_leitura) >1)
			return ultima_leitura;
	return valor_retorno;

    }
void setar_motores(){
    float raio=0;
    float MesqF=0, MdirF=0,MesqT=0, MdirT=0;
    if (controlador > 1)
        controlador = 1;
    if (controlador < -1)
        controlador = -1;
    if(controlador > 0){
        MdirF= Vmax;
        MdirT= 0.0;
        MesqF= Vmax*(1 - controlador);
        MdirT= 0.0;
    }
    else{
        MdirF= Vmax*(1 + controlador);
        MdirT= 0.0;
        MesqF= Vmax;
        MdirT= 0.0;
    }
    motor_dirFrente = MdirF;
    motor_dirTras = MdirT;
    motor_esqFrente = MesqF;
    motor_esqTras = MesqT;
    
}


int main(){
    Timer tempo, dt;
    float Dt, Teste1[7000]={};
    int i;
    i = 0;
    wait (3);
    calibrar();
    FILE *fp = fopen("/local/out.txt", "r");  // Open "out.txt" on the local file system for writing
    fscanf(fp, "%f %f %f %f %f", &kP, &kI, &kD, &Vmax, &tVOLTA);
    fclose(fp);
    wait (2);
    stop = false;
    tempo.reset();
    tempo.start();
    
/*    FILE *  fp2 = fopen("/local/out.txt", "a");  // Open "out.txt" on the local file system for writing
    fprintf(fp2, "\nNumero de leituras: %d",noLeituras);
    for (i=0; i < 8; i++)
        fprintf(fp2, "\nSensor %d: Branco %3.3f Preto %3.3f",i,calibracao_branco[i],calibracao_preto[i]);
    fclose(fp2);
*/
    Dt = 0.000787;
    dt.reset();
    dt.start();
    //flipper.attach(&setar_motores,0.018);
    while(!stop){
        P = ler_sensores();
        Dt = dt.read();
        D = (P - ultima_leitura)/Dt;
        ultima_leitura = P;
        I += P*Dt;
        controlador = kP * P + max(-2,min(kI*I,2)) + kD * D; // kI * I tem como margem tamanho limite do seguidor sobre a linha
        setar_motores();
        if (tVOLTA < tempo.read())
            break;         
		//printf("%f %f\n\r", tVOLTA,tempo.read());

			dt.stop();
        dt.reset();
        //printf("P: %.4f Controlador: %.4f\r\n", P,controlador);
        if (i < 7000){
            Teste1[i] = P;
            }
		i++;
		noLeituras++;
        dt.start();
        
        }

	parar_motores(); 
	wait(1);
	motor_esqFrente=0;
	
//    FILE *  fp2 = fopen("/local/out.txt", "a");  // Open "out.txt" on the local file system for writing
//    fprintf(fp2, "\nNumero de leituras: %d",noLeituras);
//    for (i=0; i < 8; i++)
//        fprintf(fp2, "\nSensor %d: Branco %3.3f Preto %3.3f",i,calibracao_branco[i],calibracao_preto[i]);
//    for (i=0; i < 7000; i++)
//          fprintf(fp2, "\n%f",Teste1[i]);
    //for (i=0; i < 3500; i++)
    //      fprintf(fp2, "\n%f",Teste2[i]);
//	fclose(fp2);
    
    while(1);

    }