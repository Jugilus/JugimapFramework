uniform vec4 overlayColor;
uniform sampler2D texture;


//#define SIMPLE_MULTIPLY_BLEND
#define NORMAL_PIXEL_BLEND
//#define MULTIPLY_PIXEL_BLEND
//#define LINEAR_DODGE_PIXEL_BLEND
//#define COLOR_PIXEL_BLEND

float GetLum(vec4 C)
{
	return 0.3*C.r + 0.59*C.g + 0.11*C.b;
}


vec4 BlendPixelWithPreservedOpacity(vec4 Ctop, vec4 Cbottom)
{
	
	// C = Cbottom - Cbottom*Ctop.a + Ctop.a * BLEND
	
	vec4 C = Cbottom;
	
	
#ifdef SIMPLE_MULTIPLY_BLEND

	C = Ctop * Cbottom;
	
#endif
	
	
#ifdef NORMAL_PIXEL_BLEND

	//BLEND = Ctop
	C = Cbottom - Cbottom*Ctop.a + Ctop.a * Ctop;
	
#endif

#ifdef MULTIPLY_PIXEL_BLEND

	//BLEND = Ctop*Cbottom
	C = Cbottom - Cbottom*Ctop.a + Ctop.a * Ctop*Cbottom;
	
#endif

#ifdef LINEAR_DODGE_PIXEL_BLEND

	// BLEND = min(vec4(1.0), Ctop+Cbottom)
	C = Cbottom - Cbottom*Ctop.a + Ctop.a * min(vec4(1.0), Ctop+Cbottom);
	
#endif


#ifdef COLOR_PIXEL_BLEND

	//BLEND = SetLum(Ctop, Lum(Cbottom))
	
	float d = GetLum(Cbottom) - GetLum(Ctop);
	C = Ctop + d;
	float l = GetLum(C);
	float n = min(C.r, min(C.g,C.b));
	float x = max(C.r, max(C.g,C.b));
	if(n<0.0){
		C = l + ((C-l)*l)/(l-n);
	}
	if(x>1.0){
		C = l + ((C-l)*(1.0-l))/(x-l);
	}
	
	C = Cbottom - Cbottom*Ctop.a + Ctop.a * C;

#endif
	
	C.a = Cbottom.a;
	
	return C;
}


void main(){

	gl_FragColor = BlendPixelWithPreservedOpacity(overlayColor, texture2D( texture, gl_TexCoord[0].xy) * gl_Color);

}





