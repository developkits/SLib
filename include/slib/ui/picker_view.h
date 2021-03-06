/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CHECKHEADER_SLIB_UI_PICKER_VIEW
#define CHECKHEADER_SLIB_UI_PICKER_VIEW

#include "definition.h"

#include "view.h"
#include "motion_tracker.h"

namespace slib
{

	class SLIB_EXPORT PickerView : public View
	{
		SLIB_DECLARE_OBJECT
		
	public:
		PickerView();
		
		~PickerView();

	public:
		sl_uint32 getItemsCount();
		
		virtual void setItemsCount(sl_uint32 n, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		void removeAllItems(UIUpdateMode mode = UIUpdateMode::Redraw);
		
		
		String getItemValue(sl_uint32 index);
		
		virtual void setItemValue(sl_uint32 index, const String& value);
		
		List<String> getValues();
		
		virtual void setValues(const List<String>& values);
		
		
		String getItemTitle(sl_uint32 index);
		
		virtual void setItemTitle(sl_uint32 index, const String& title, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		List<String> getTitles();
		
		virtual void setTitles(const List<String>& values, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		
		virtual void selectIndex(sl_uint32 index, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		virtual void selectValue(const String& value, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		sl_uint32 getSelectedIndex();
		
		String getSelectedValue();
		
		String getSelectedTitle();
		
		
		Color getTextColor();
		
		virtual void setTextColor(const Color& color, UIUpdateMode mode = UIUpdateMode::Redraw);
		
		
	public:
		SLIB_PROPERTY(AtomicFunction<void(PickerView*, sl_uint32)>, OnSelectItem)
		
	protected:
		void onDraw(Canvas* canvas) override;
		
		void onMouseEvent(UIEvent* ev) override;
		
		virtual void onSelectItem(sl_uint32 index);
		
	public:
		Ref<ViewInstance> createNativeWidget(ViewInstance* parent) override;
		
		virtual void dispatchSelectItem(sl_uint32 index);
		
	private:
		void _selectIndexInner(sl_int32 index);
		
		sl_uint32 _getCircularIndex(sl_int32 index);
		
		sl_ui_len _getLineHeight();
		
		void _flow(sl_ui_pos offset);
		
		void _startFlow(sl_real speed);
		
		void _stopFlow();
		
		void _animationCallback(Timer* timer);
		
		
		void _getSelectedIndex_NW();
		
		void _select_NW(sl_uint32 index);
		
		void _refreshItemsCount_NW();
		
		void _refreshItemsContent_NW();
		
		void _setItemTitle_NW(sl_uint32 index, const String& title);
		
		void _setFont_NW(const Ref<Font>& font) override;
		
	protected:
		AtomicList<String> m_values;
		AtomicList<String> m_titles;
		sl_uint32 m_indexSelected;
		Color m_textColor;
		
		sl_uint32 m_linesHalfCount;
		sl_bool m_flagCircular;
		
		sl_ui_pos m_yOffset;
		
		Ref<MotionTracker> m_motionTracker;
		Ref<Timer> m_timerFlow;
		sl_real m_speedFlow;
		Time m_timeFlowFrameBefore;
		
	};

}

#endif
