#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <lcd.h>
#include <mcp23017.h>

#define MCP_BASE         100

// GPB0 = 8
#define MCP_RS           (MCP_BASE + 8) 

#define MCP_E0           (MCP_BASE + 10)
#define MCP_E1           (MCP_BASE + 9)

#define MCP_DB4          (MCP_BASE + 12)
#define MCP_DB5          (MCP_BASE + 13)
#define MCP_DB6          (MCP_BASE + 14)
#define MCP_DB7          (MCP_BASE + 15)

#define LCD_BITS  4
#define LCD_ROWS  2
#define LCD_COLS  16

// Global lcd handle:
static int lcdHandle ;

/*
 * usage:
 *********************************************************************************
 */

int usage (const char *progName)
{
  fprintf (stderr, "Usage: %s [-s slave_addr][-l lcd][-i][-r row][-c column][-C] string\n", progName) ;
  fprintf (stderr, "       %s -b [-s slave_addr][-l lcd][-i][-r row][-c column][-C] byte(s) ...\n", progName) ;
  return EXIT_FAILURE ;
}

/*
 * The works
 *********************************************************************************
 */

int main (int argc, char *argv[])
{
  int i,j ;
  uint8_t slave = 0x20;
  int row,col,lcd;
  int lcd_clear = 0;
  int bytemode = 0;
  int lcd_init = 0;
  char buf[4][40];
  int result;

  row = col = lcd = 0;

  while((result=getopt(argc,argv,"l:r:c:s:bCi")) != -1) {
    switch(result){
    case 'l':
      lcd = (int)strtol(optarg,NULL,0);
      if (lcd < 0 || lcd > 1) {
	fprintf(stderr,"Select lcd: -l should be 0 or 1.\n");
	usage(argv[0]);
      }
#ifdef DEBUG
      fprintf(stderr,"%c %d\n",result,lcd);
#endif
      break;
    case 'r':
      row = (int)strtol(optarg,NULL,0);
      if (row < 0 || row >= LCD_ROWS) {
	fprintf(stderr,"Row: row = 0 or 1\n");
	usage(argv[0]);
      }
#ifdef DEBUG
      fprintf(stderr,"%c %d\n",result,row);
#endif
      break;
    case 'c':
      col = (int)strtol(optarg,NULL,0);
      if (col < 0 || col >= LCD_COLS) {
	fprintf(stderr,"Col: 0 <= col <= %d\n",LCD_COLS);
	usage(argv[0]);
      }
#ifdef DEBUG
      fprintf(stderr,"%c %d\n",result,col);
#endif
      break;
    case 's':
      slave = (uint8_t)strtol(optarg,NULL,0);
#ifdef DEBUG
      fprintf(stderr,"%c 0x%x\n",result,slave);
#endif
      break;
    case 'b':
      bytemode = 1;
#ifdef DEBUG
      fprintf(stderr,"%c byte mode\n",result);
#endif
      break;
    case 'C':
      lcd_clear = 1;
#ifdef DEBUG
      fprintf(stderr,"%c clear lcd\n",result);
#endif
      break;
    case 'i':
      lcd_init = 1;
#ifdef DEBUG
      fprintf(stderr,"%c clear lcd\n",result);
#endif
      break;
    }
  }

  i = 0; j = 0;
  for (;optind<argc;optind++) {
    if (bytemode) {
      buf[i][j] = (char)strtol(argv[optind],NULL,0);
#ifdef DEBUG
      fprintf(stderr,"argv: %s -> %c\n",argv[optind],buf[i][j]);
#endif
      j++;
      buf[i][j+1] = '\0';
      if (j > 15) {
	i++;
	j=0;
      }
    } else {
      if (i == 0) {
	strcpy(buf[0],argv[optind]);
	i++;
      } else if (i==1) {
	strcpy(buf[1],argv[optind]);
	i++;
      }
#ifdef DEBUG
      fprintf(stderr,"argv: %s\n",argv[optind]);
#endif
    }
  }

  wiringPiSetup() ;
  mcp23017Setup(MCP_BASE, slave) ;

  if (lcd_init == 1) {
    lcdHandle = lcdInit(LCD_ROWS,LCD_COLS,LCD_BITS,
			MCP_RS,
			(lcd == 0 ? MCP_E0 : MCP_E1),
			MCP_DB4,MCP_DB5,MCP_DB6,MCP_DB7,0,0,0,0) ;
    if (lcdHandle < 0)  {
      fprintf (stderr, "Error: lcdInit failed\n") ;
      return -1 ;
    }
  } else {
    lcdHandle = lcdOpen(LCD_ROWS,LCD_COLS,LCD_BITS,
			MCP_RS,
			(lcd == 0 ? MCP_E0 : MCP_E1),
			MCP_DB4,MCP_DB5,MCP_DB6,MCP_DB7,0,0,0,0) ;
    if (lcdHandle < 0)  {
      fprintf (stderr, "Error: lcdOpen failed\n") ;
      return -1 ;
    }
  }


  if (lcd_clear == 1) {
      lcdClear(lcdHandle) ;
      lcdHome(lcdHandle) ;
#ifdef DEBUG
      fprintf(stderr,"Clear:\n");
#endif
  }

  if (strlen(buf[0]) > 0) {
    lcdPosition (lcdHandle, col, row); 
    lcdPuts (lcdHandle, buf[0]) ;
#ifdef DEBUG
    fprintf(stderr,"Wrote: l=%d c=%d r=%d %s\n",lcd,col,row,buf[0]);
#endif
    if (i > 0) {
      lcdPosition (lcdHandle, col, (row+1)); 
      lcdPuts (lcdHandle, buf[1]) ;
#ifdef DEBUG
      fprintf(stderr,"Wrote: l=%d c=%d r=%d %s\n",lcd,col,row+1,buf[1]);
#endif
    }
  }
  return 0 ;
}
