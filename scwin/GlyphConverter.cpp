#include "pch.h"
#include "GlyphConverter.h"


namespace scwin
{

	GlyphConverter::GlyphConverter()
	{
	}

	Object ^ GlyphConverter::Convert(Object ^value, TypeName targetType, Object ^parameter, String ^language)
	{
		IBox<Boolean>^ isExpanded = dynamic_cast<IBox<Boolean>^>(value);
		if (isExpanded != nullptr && isExpanded->Value)
		{
			return ExpandedGlyph;
		}
		else
		{
			return CollapsedGlyph;
		}
	}

	Object ^ GlyphConverter::ConvertBack(Object ^value, TypeName targetType, Object ^parameter, String ^language)
	{
		throw ref new NotImplementedException();
	}

}