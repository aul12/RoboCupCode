// arctan.h: Header für Arcus-Tangens Näherung (ca. 6% max. Abweichung, mit Pi=3 ca. 11%)

#ifdef _cplusplus
	extern "C" {
#endif

#define PI 3.141592653589
//#define PI 3

inline uint8_t arctan(float x)
{
    if(BETRAG(x)<=1.0)
        return (uint8_t)((x/(1+0.28*x*x))*180/PI) ;
    else if(x>0)
		return (uint8_t)((PI/2-x/(x*x+0.28))*180/PI);
	else
		return (uint8_t)((-PI/2-x/(x*x+0.28))*180/PI);
}


#ifdef _cplusplus
	}
#endif
