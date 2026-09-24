#ifndef WXIMAGEUTILS_H
#define WXIMAGEUTILS_H
#include <wx/bitmap.h>
#include <wx/image.h>

// Reduce (o amplía) un mapa de bits a la altura pedida conservando la
// proporción, con filtrado de calidad. Las imágenes de la interfaz se
// incrustan en alta resolución (el logo es de 954x343) y se ajustan al DPI
// del monitor en tiempo de ejecución, así quedan nítidas en cualquier escala.
inline wxBitmap bitmap_scaled_to_height(const wxBitmap &bmp, int height) {
	if (!bmp.IsOk() || height <= 0 || bmp.GetHeight() == height) return bmp;
	wxImage img = bmp.ConvertToImage();
	int width = (int) (img.GetWidth() * (double) height / img.GetHeight() + 0.5);
	img.Rescale(width < 1 ? 1 : width, height, wxIMAGE_QUALITY_HIGH);
	return wxBitmap(img);
}

#endif
