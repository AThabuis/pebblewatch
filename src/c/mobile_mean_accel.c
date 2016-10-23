#include <pebble.h>
#include "mobile_mean_accel.h"

void mobile_mean_accel(AccelData *data, uint32_t num_samples, int16_t *mag)
{
    static int32_t last_value1 = 0;  //second last value for the mobile mean for the last call
    static int32_t last_value2 = 0; //last value for the mobile mean for the last call 
    
    int32_t future_value1 = 0;
    int32_t future_value2 = 0; //pour ne pas écraser les données, dû au sens de rotation du buffer
  
    int32_t data_mag[num_samples];
    uint16_t i = 0;
  
    //use of abs() because oddly the multiplication keeps the sign of the variable here
    for(i=0; i<num_samples; i++)//we make a tabble of magnitude
    {
        /*data_mag[i] = (int32_t) sqrt((abs((data[i].x)*(data[i].x)) 
                                   + abs((data[i].y)*(data[i].y)) 
                                   + abs((data[i].z)*(data[i].z))) / 3);*/
        data_mag[i] = abs((data[i].x)*(data[i].x)) 
                                   + abs((data[i].y)*(data[i].y)) 
                                   + abs((data[i].z)*(data[i].z));
    }
  
    future_value1 = data_mag[num_samples - 2];//second-last data of the table
    future_value2 = data_mag[num_samples - 1];//last data of the table
  
    for(i=0; i<num_samples; i++)
    {
      /*La moyenne mobile part du premier élément du buffer actuel pour aller au dernier élément.
      La moyenne se faisant sur 3 éléments, lorsque l'on se trouve dans le haut (Premier élément puis le second 
      élément) on utilise les deux derniers éléments du précédent buffer (nommé last_value1/2) pour 
      effectuer la moyenne */
      if(i==0)//1er élément du buffer (moy avec les deux dernières valeurs de l'ancien buffer)
      {
         data_mag[0]=
            (last_value1 + last_value2 + data_mag[0]) / 3;
      }
      else if(i == 1)//2eme élément du buffer (moy avec la dernière valeur de l'ancien buffer et le premier élément du buffer actuel)
      {
          data_mag[1]=
            (last_value2  + data_mag[0] + data_mag[1]) / 3;
      }
      else//Pour le reste du buffer
      {
          data_mag[i]=
            (data_mag[i-2] + data_mag[i-1] + data_mag[i]) / 3;
      }
    }
    
    //On attribut les deux dernières valeurs de l'ancien buffer sauvegardées à last_value
    last_value1=future_value1;
    last_value2=future_value2;  
  
    *mag = (data_mag[num_samples-1]) >> 9; //pour etre aux alentours de 2047 à 1G (=32767 / 16)
    //magnitude a pour valeur le dernier élément du tableau
}
