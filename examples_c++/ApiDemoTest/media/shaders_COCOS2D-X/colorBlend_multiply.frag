uniform vec4 overlayColor;
//uniform sampler2D CC_Texture0;

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

//#define SIMPLE_MULTIPLY_BLEND
//#define NORMAL_PIXEL_BLEND
#define MULTIPLY_PIXEL_BLEND
//#define LINEAR_DODGE_PIXEL_BLEND
//#define COLOR_PIXEL_BLEND

float GetLum(vec4 C)
{
	return 0.3*C.r + 0.59*C.g + 0.11*C.b;
}


vec4 BlendPixelWithPreservedOpacity(vec4 Ctop, vec4 Cbottom)
{
	vec4 C = Cbottom;
	
	
#ifdef SIMPLE_MULTIPLY_BLEND

	C = Ctop * Cbottom;
	C.a = Cbottom.a;
	
#endif
	
	
#ifdef NORMAL_PIXEL_BLEND

	float A = Cbottom.a + Ctop.a - (Cbottom.a * Ctop.a);
	C = (Ctop.a*Ctop + (Cbottom.a*Cbottom)*(1.0-Ctop.a))/A;
	C.a = Cbottom.a;
	
#endif

#ifdef MULTIPLY_PIXEL_BLEND

	float A = Cbottom.a + Ctop.a - (Cbottom.a * Ctop.a);
	C = (1.0-Ctop.a/A)*Cbottom + (Ctop.a/A)*((1.0-Cbottom.a)*Ctop + Cbottom.a * Ctop*Cbottom);
	C = C * Cbottom.a;
	C.a = Cbottom.a;
	
#endif

#ifdef LINEAR_DODGE_PIXEL_BLEND

	float A = Cbottom.a + Ctop.a - (Cbottom.a * Ctop.a);
	C = (1.0-Ctop.a/A)*Cbottom + (Ctop.a/A)*((1.0-Cbottom.a)*Ctop + Cbottom.a * min(vec4(1.0), Ctop+Cbottom));
	C.a = Cbottom.a;
	
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
	
	float A = Cbottom.a + Ctop.a - (Cbottom.a * Ctop.a);
	C = (1.0-Ctop.a/A)*Cbottom + (Ctop.a/A)*((1.0-Cbottom.a)*Ctop + Cbottom.a * C);
	C.a = Cbottom.a;

#endif

	return C;
}


void main(){

	gl_FragColor = BlendPixelWithPreservedOpacity(overlayColor, texture2D(CC_Texture0, v_texCoord) * v_fragmentColor);
	
}





