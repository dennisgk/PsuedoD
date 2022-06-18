#include "QuickMath.h"

float absf_s(float f) {
    union {
        float f;
        uint32_t i;
    } u;
    u.f = f;
    u.i &= 0x7fffffff;
    return u.f;
}

float negf_s(float f){
    union {
        float f;
        uint32_t i;
    } u;
    u.f = f;
    u.i ^= 0x80000000;
    return u.f;
}


float cosf(float rad){
    
    if(rad < negHalfPI){
        rad = negPI - rad;
    }
    else if(rad > halfPI){
        rad = PI - rad;
    }

    rad = rad * 1.0138f;
    
    float radrad = rad * rad;

    return (1 - (radrad / 2.0f) + ((radrad * radrad) / 24.0f));
}

float sinf(float rad){
    
    if(rad < negHalfPI){
        rad = negPI - rad;
    }
    else if(rad > halfPI){
        rad = PI - rad;
    }

    rad = rad * 1.0138f;
    
    float radrad = rad * rad;
    float radradrad = rad * rad * rad;

    return (rad - ((radradrad) / 6.0f) + ((radradrad * radrad) / 120.0f));
}

/*float sinf(float rad){
    float m = (fourOverPI * rad) + (negFourOverPISquared * rad * absf_s(rad));
    return ((PIp * ((m * absf_s(m)) - m)) + m);
}*/

float cosf_s(float rad){
    //return sinf_s(rad + halfPI);
    if(rad > PI){
        while(rad > PI){
            rad = rad - doublePI;
        }
    }
    else{
        if(rad < negPI){
            while(rad < negPI){
                rad = rad + doublePI;
            }
        }
    }

    return cosf(rad);
}

float sinf_s(float rad){
    if(rad > PI){
        while(rad > PI){
            rad = rad - doublePI;
        }
    }
    else{
        if(rad < negPI){
            while(rad < negPI){
                rad = rad + doublePI;
            }
        }
    }

    return sinf(rad);
}

float sinf_sc(float* rad){
    if(*rad > PI){
        while(*rad > PI){
            *rad = *rad - doublePI;
        }
    }
    else{
        if(*rad < negPI){
            while(*rad < negPI){
                *rad = *rad + doublePI;
            }
        }
    }

    return sinf(*rad);
}

float tanf(float rad){
    float z = (1.0f - ((twoOverPI * rad) * (twoOverPI * rad)));
    return ((twoOverPI * rad) * ((-0.0187108f * z) + 0.31583526f + (1.27365776f / z)));
}

float tanf_s(float rad){
    if(rad > halfPI){
        while(rad > halfPI){
            rad = rad - PI;
        }
    }
    else{
        if(rad < negHalfPI){
            while(rad < negHalfPI){
                rad = rad + PI;
            }
        }
    }

    return tanf(rad);
}

/*float tanf_s(float rad){
    if(rad > PI){
        while(rad > PI){
            rad = rad - doublePI;
        }
    }
    else{
        if(rad < negPI){
            while(rad < negPI){
                rad = rad + doublePI;
            }
        }
    }

    return (sinf(rad) / cosf(rad));
}*/

float cotanf_s(float rad){
    if(rad > PI){
        while(rad > PI){
            rad = rad - doublePI;
        }
    }
    else{
        if(rad < negPI){
            while(rad < negPI){
                rad = rad + doublePI;
            }
        }
    }

    return (cosf(rad) / sinf(rad));
}

float arcsinf(float rad){
    //float radrad = rad * rad;
    //return (halfPI - (sqrt_f(1-rad)*(1.5707288f + (-0.2121144f * rad) + (0.0742610f * radrad) + (-0.0187293f * rad * radrad))));
    return (halfPI - (sqrt_f(1.0f-rad)*(1.5707288f + (-0.2121144f * rad))));
}

float arcsinf_s(float rad){
    if(rad > 1.0f){
        return arcsinf(1.0f);
    }
    else if(rad < 0.0f){
        if(rad < -1.0f){
            return negf_s(arcsinf(1.0f));
        }
        else{
            return negf_s(arcsinf(negf_s(rad)));
        }
    }
    else{
        return arcsinf(rad);
    }
}

/*float arctanf_s(float rad){
    return arcsinf_s((rad)*(isqrt_f(1.0f + (rad*rad))));
}*/
float arctanf_s(float rad){
    return ((oneFourthPI * rad) + ((0.285f * rad) * (1.0f - absf_s(rad))));
}

float arccotf_s(float rad){
    float invRad = 1.0f / rad;
    return arcsinf_s((invRad)*(isqrt_f(1.0f+((invRad)*(invRad)))));
}

float arccosf_s(float rad){
    return ((negf_s(arcsinf_s(rad))) + halfPI);
}

float isqrt_f( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       
	i  = 0x5f3759df - ( i >> 1 );               
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   

	return y;
}

float sqrt_f(float x)
{
    return (x * isqrt_f(x));
}

float flmod(float og, float mod){
    og = absf_s(og);
    while(og > mod){
        og -= mod;
    }
    return og;
}

void swap(void** a, void** b)
{
    void* t = *a;
    *a = *b;
    *b = t;
}

int partition (void** arr, int low, int high, int (*compare)(void*, void*))
{
    void* pivot = arr[high]; // pivot
    int i = (low - 1); // Index of smaller element and indicates the right position of pivot found so far
 
    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than the pivot
        if ((*compare)(pivot, arr[j]) > 0)
        {
            i++; // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(void** arr, int low, int high, int (*compare)(void*, void*))
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
        at right place */
        int pi = partition(arr, low, high, compare);
 
        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1, compare);
        quickSort(arr, pi + 1, high, compare);
    }
}

float toTrigBounds(float rad){
    if(rad > PI){
        while(rad > PI){
            rad = rad - doublePI;
        }
    }
    else{
        if(rad < negPI){
            while(rad < negPI){
                rad = rad + doublePI;
            }
        }
    }
    return rad;
}

float toTrigBoundsB(float rad, unsigned char* quadrant){
    if(rad > PI){
        while(rad > PI){
            rad = rad - doublePI;
        }
    }
    else{
        if(rad < negPI){
            while(rad < negPI){
                rad = rad + doublePI;
            }
        }
    }

    if(rad >= 0.0f && rad <= halfPI){
        *quadrant = 1;
    }
    else if(rad > halfPI && rad <= PI){
        *quadrant = 2;
    }
    else if(rad < 0.0f && rad >= negHalfPI){
        *quadrant = 4;
    }
    else{
        *quadrant = 3;
    }

    return rad;
}