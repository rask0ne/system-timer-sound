		#include <stdio.h>
	#include <conio.h>
	#include <dos.h>
	#include <string.h>

	void sound(int, int);
	void tm_sound(float freq, int time);
	void tm_delay(int ticks);
	void print_channels();
	void randomize();
	void random_set(int number);
	int random_get(void);
	void int_to_int(int num, char* str);

	// Массив частот для мелодии
	float mary[] =
	{
	  392.0/4, 18.2, 392.0/4, 18.2, 392.0/4, 18.2, 311.1/4, 466.2/4, 392.0/4, 18.2, 311.1/4, 466.2/4, 392.0/4, 18.2,
	  293.7/2, 18.2, 293.7/2, 18.2, 293.7/2, 18.2, 311.1/2, 466.2/4, 392.0/4, 18.2, 311.1/4, 466.2/4, 392.0/4, 18.2, 0
	 // 392.0, 392.0/4, 392.0/4, 392.0, 370.0, 349.2, 329.6, 311.1, 329.6, 415.3, 277.2, 261.7, 493.9, 466.2, 440.0, 466.2, 311.1, 370.0, 466.2, 392.0, 466.2, 293.7, 392.0, 0
	};

    /*

	G G G Eb Bb G Eb Bb G

	(up above tuning C) D D D Eb (back down) Bb G Eb Bb G

	high G low GG high G F# F E Eb E Ab C# C B Bb A Bb Eb F# Bb G Bb

	D G (octave down) GG G F# F E Eb E Ab C# C B Bb A Bb Eb Bb G

	Eb Bb G

    До  	C; do
    Ре	    D; re
    Ми	    E; mi
    Фа	    F; fa
    Соль	G; sol
    Ля	    A; la
    Си	    B; si

    */

	// Массив длительностей
	int del[] =
	{
	  12, 1, 12, 1, 12, 2,  11, 6, 6, 4, 10, 6, 6, 4,
	  12, 1, 12, 1, 12, 2,  11, 6, 6, 4, 10, 6, 6, 4, 1
	 // 12, 6, 6, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 6, 6, 1
	};

	int main(void)
	{
	  int i;

	  for(i=0 ;mary[i] != 0 ;i++)
	    tm_sound(mary[i], del[i]);
	  print_channels();
	  randomize();

		getch();

	  return 0;
	}

	/**
	*.Name         tm_sound
	*.Title        Формирование тона заданной длительности
	*
	*.Descr        Эта функция предназначена для генерации
	*              на громкоговорителе тона заданной
	*              длительности и частоты
	*
	*.Proto        void tm_sound(int freq, int time);
	*
	*.Params       int freq - частота в герцах;
	*              int time - длительность в периодах работы
	*               таймера
	**/
	void tm_sound(float freq, int time)
	{
	  int cnt;

	  // Задаем режим канала 2 таймера
	  outp(0x43, 0xb6);

	  // Вычисляем задержку для загрузки в
	  // регистр счетчика таймера
	  cnt = (int)(1193180L / freq);

	  // Загружаем регистр счетчика таймера - сначала
	  // младший, затем старший байты
	  outp(0x42, cnt & 0x00ff);
	  outp(0x42, (cnt &0xff00) >> 8);

	  // Включаем громкоговоритель. Сигнал от
	  // канала 2 таймера теперь будет проходить
	  // на вход громкоговорителя
	  outp(0x61, inp(0x61) | 3);

	  // Выполняем задержку.
	  tm_delay(time);

	  // Выключаем громкоговоритель.
	  outp(0x61, inp(0x61) & 0xfc);
	}

	/**
	*.Name         tm_delay
	*.Title        Формирование задержки по таймеру
	*
	*.Descr        Эта функция формирует задержку, используя
	*              системный таймер
	*
	*.Proto        void tm_delay(int ticks)
	*
	*.Params       int ticks - величина задержки в периодах работы таймера
	**/

	void tm_delay(int ticks)
	{
	  _asm
	  {
	  _asm  push si

	  _asm  mov  si, ticks
	  _asm  mov  ah, 0
	  _asm  int  1ah

	  _asm  mov  bx, dx
	  _asm  add  bx, si

	delay_loop:

	  _asm  int  1ah
	  _asm  cmp  dx, bx
	  _asm  jne  delay_loop

	  _asm  pop  si
	  }
	}

	void print_channels()
	{
		unsigned i, number;
		char bits[9];

	  printf("\n\Channel 0\n-------\n");

	  // Читаем слово состояния канала,
	  // команда 0xe2 = 11100010B
	    outp(0x43, 0xe2);
		int_to_int(inp(0x40), bits);

	  printf("\nCurrent statement word: %s\n",
	    bits);

	  // Читаем текущее состояние регистра счетчика
	  // канала. Для этого вначале выдаем команду CLC
	  // для канала 0. Код этой команды - 0x00
	  outp(0x43, 0x00);

	  // Вводим младший и старший байты счетчика
	  // и отображаем его.
	  i = inp(0x40);
	  i = (inp(0x40) << 8) + i;

	  printf("\nRegister CE:       %04.4X",i);

	  // Повторяем те же действия для 1 и 2 каналов.
	  printf("\n\nChannel 1\n-------\n");
	  outp(0x43, 0xe4);
	  int_to_int(inp(0x41), bits);
	  printf("\nCurrent statement word: %s",bits);
	  outp(0x43, 0x40);
	  i = inp(0x41);
	  i = (inp(0x41) << 8) + i;
	  printf("\nRegister CE:       %04.4X",i);

	  printf("\n\nChannel 2\n-------\n");
	  outp(0x43, 0xe8);
	  int_to_int(inp(0x42), bits);
	  printf("\nCurrent statement word: %s",bits);
	  outp(0x43, 0x80);
	  i = inp(0x42);
	  i = (inp(0x42) << 8) + i;
	  printf("\nRegister CE:       %04.4X",i);

	}

	void randomize()
	{
		int i, j, number;

		printf("\nEnter number: ");
		scanf("%d", &number);
		printf("\n");

		random_set(number);
		j = random_get();
		printf("\nRandom number:          %d", j);

	}

	void random_set(int bound)
	{
	  // Устанавливаем режим 3 для второго канала таймера
	  outp(0x43, 0xb6);

	  // Загружаем регистр счетчика таймера - сначала
	  // младший, затем старший байты
	  outp(0x42, bound & 0x00ff);
	  outp(0x42, (bound &0xff00) >> 8);

	  // Разрешаем работу канала
	  outp(0x61, inp(0x61) | 1);
	}

	/**
	*.Name         rnd_get
	*.Title        Получение от таймера случайного числа
	*
	*.Descr        Эта функция получает случайное число от
	*              таймера, который был предварительно
	*              проинициализирован функцией rnd_set
	*
	*.Proto        int rnd_get(void)
	*
	*.Params       Отсутствуют.
	*
	*.Return       Случайное число в диапазоне от 0, до
	*              уменьшенного на 1 значения, заданного в
	*              качестве параметра функции rnd_set().
	**/

	int random_get(void)
	{
	  int i;

	  // Выдаем команду CLC для фиксирования
	  // текущего значения регистра канала 2 таймера
	  outp(0x43, 0x86);

	  // Вводим младший и старший байты счетчика
	  i = inp(0x42);
	  i = (inp(0x42) << 8) + i;

	  return(i);
	}

void int_to_int(int num, char* str)
{

	*(str+8) = '\0';
 int mask = 0x80 << 1;
  while(mask >>= 1)
       *str++ = !!(mask & num) + '0';


}