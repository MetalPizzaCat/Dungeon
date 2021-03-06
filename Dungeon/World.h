#pragma once
#include "Base/BaseObject.hpp"
#include "Base/UI/UIBase.hpp"
#include "Base/Macros.h"
#include "Base/Item/Item.hpp"
#include "Base/Input/InputSystem.hpp"

namespace Engine
{
	struct Cell
	{
	public:
		Vector Location;

		bool Occupied = false;

		int OccupantId = -1;
	};
	
	/*
	* World is the class that manages object updates, ui updates etc.
	* It represents the game itself
	* Note: Level change is also handled by world as it just clears object pool and loads new ones from layout
	*/
	class CWorld
	{
	private:
		unsigned int spawnCounter = 0;

		//Array of debug messages that will be printed out when update is called
		//Cleared after each print out
		Array<String>debugOutputMessages = Array<String>();

		Array<Engine::Item>defaultItemData = Array<Engine::Item>();

		Array<Cell>occupanceData = Array<Cell>();
	protected:
		//File will be loaded into the memory in the start of the game and then data will be read from here
		String entityFileText;
	public:
		CWorld();
		//system for getting current input
		Input::CInputSystem Input;

		/*Get's cell data for a specific location
		If game has no record of that cell it will be added*/
		Cell GetCellData(Vector loc);

		/*Set data for a cell in this location
		NOTE:There are no chekcs so any object can change any cell's data*/
		void SetCellData(Vector loc, Cell cell);

		int CurrenInput = -1;

		int MaxDebugMessageCount = -1;

		UI::CUIBase* DebugOutput = nullptr;

		CBaseObject* GetObjectByObjectId(unsigned int id);

		//Default item data is basically the default thing
		//this function uses std::find_if for search and basically iterates over whole item array so avoid using in big amounts in very close parts
		Engine::Item GetItemDefaultData(String itemName, bool& hasData)const;

		//This value is set by object if there was an input that has effect on gameplay
		//Reset after all update functions are called
		bool GameplayUpdate = false;

		Array<Engine::CBaseObject*> Objects = Array<Engine::CBaseObject*>();

		Array<Engine::UI::CUIBase*>UIElements = Array<Engine::UI::CUIBase*>();

		void AddDebugMessage(String msg);

		//Update UI calls ui to proccess input and them be drawn
		void UpdateUI();

		//Loads entity file into the memory for future access
		bool LoadEntityFile();

		//Fills defaultItemData with data from Items.dat
		bool LoadItemFile();

		//Call process input functions in objects
		void ProcessInput();

		//This function removes actors who's Valid check returns false
		void KillActorsMarkedForDelete();

		void Update();

		template<class Class, class ... Args>
		Class* SpawnObject(Args ... args);

		template<class Class, class ... Args>
		Class* CreateUI(UI::CUIBase* parent = nullptr,Args...args);
	};

	template<class Class, class ...Args>
	inline Class* CWorld::SpawnObject(Args ... args)
	{
		Class* obj = new Class(args...);
		if (obj)
		{
			obj->World = this;
			obj->id = spawnCounter;
			spawnCounter++;
			Objects.push_back(obj);

			return obj;
		}
		else
		{
			return nullptr;
		}
	}
	template<class Class, class ...Args>
	inline Class* CWorld::CreateUI(UI::CUIBase*parent, Args ...args)
	{
		Class* ui = new Class(args...);
		if (ui)
		{
			if (parent)
			{
				parent->ChildrenUI.push_back(ui);
				ui->Parent = parent;
			}
			UIElements.push_back(ui);
			return ui;
		}
		else
		{
			return nullptr;
		}
	}
}

#define INPUT_SYSTEM World->Input