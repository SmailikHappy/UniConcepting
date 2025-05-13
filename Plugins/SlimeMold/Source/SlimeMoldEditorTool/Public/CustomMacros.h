#pragma once

#define CHANGE_EVENTS_OneProperty(ObjectToBeChanged, ObjectClassName, Property1)\
		FPropertyChangedEvent ChangeEvent(FindFProperty<FProperty>(ObjectToBeChanged->GetClass(), GET_MEMBER_NAME_CHECKED(ObjectClassName, Property1)));\
		ObjectToBeChanged->PostEditChangeProperty(ChangeEvent);

#define CHANGE_EVENTS_TwoProperties(ObjectToBeChanged, ObjectClassName, Property1, Property2)\
		FPropertyChangedEvent ChangeEvent(FindFProperty<FProperty>(ObjectToBeChanged->GetClass(), GET_MEMBER_NAME_CHECKED(ObjectClassName, Property1)));\
		ObjectToBeChanged->PostEditChangeProperty(ChangeEvent);\
		FPropertyChangedEvent ChangeEvent2(FindFProperty<FProperty>(ObjectToBeChanged->GetClass(), GET_MEMBER_NAME_CHECKED(ObjectClassName, Property2)));\
		ObjectToBeChanged->PostEditChangeProperty(ChangeEvent2);

#define MODIFY(ObjectToBeChanged, ChangeFunction, CHANGE_EVENTS)\
		ObjectToBeChanged->Modify();\
		ChangeFunction;\
		CHANGE_EVENTS\
		ObjectToBeChanged->MarkPackageDirty();\
