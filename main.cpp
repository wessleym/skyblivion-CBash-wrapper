#include <sstream>
#include <iostream>
#include <boost/regex.hpp>
#include "CBash/src/Skyblivion/Skyblivion.h"

using namespace Skyblivion;

void convertACTI(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->ACTI.pool.MakeRecordsVector(obRecords);
	skyblivionFile->ACTI.pool.MakeRecordsVector(skbRecords);
	geckFile->ACTI.pool.MakeRecordsVector(skbRecords);
	
	std::vector<Sk::ACTIRecord*> targets = std::vector<Sk::ACTIRecord*>();
	log_debug << obRecords.size() << " ACTIs found in oblivion file.\n";
	for(uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::ACTIRecord *p = (Ob::ACTIRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find ACTI EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::ACTIRecord* target = reinterpret_cast<Sk::ACTIRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to ACTI: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->ACTI.pool.construct(targets.at(i), NULL, false);
	}

	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->ACTI.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
			Sk::ACTIRecord* acti = (Sk::ACTIRecord*) modRecords.at(i);
			acti->IsChanged(true);
	}

}

void convertCONT(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->CONT.pool.MakeRecordsVector(obRecords);
	skyblivionFile->CONT.pool.MakeRecordsVector(skbRecords);
	geckFile->CONT.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::CONTRecord*> targets = std::vector<Sk::CONTRecord*>();
	log_debug << obRecords.size() << " CONTs found in oblivion file.\n";

	//WTM:  Change:  Added:
	std::vector<Record*, std::allocator<Record*>> refrRecords;
	skyblivionFile->CELL.refr_pool.MakeRecordsVector(refrRecords);

	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::CONTRecord *p = (Ob::CONTRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find CONT EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::CONTRecord* target = reinterpret_cast<Sk::CONTRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				SkyblivionScript skyblivionScript = converter.getSkyblivionScript(script);
				Script* convertedScript = converter.createVirtualMachineScriptBySkyblivionScript(skyblivionScript);
				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to CONT: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->CONT.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->CONT.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
			Sk::CONTRecord* acti = (Sk::CONTRecord*) modRecords.at(i);
			acti->IsChanged(true);
	}
	


}

void convertDOOR(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->DOOR.pool.MakeRecordsVector(obRecords);
	skyblivionFile->DOOR.pool.MakeRecordsVector(skbRecords);
	geckFile->DOOR.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::DOORRecord*> targets = std::vector<Sk::DOORRecord*>();
	log_debug << obRecords.size() << " DOORs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::DOORRecord *p = (Ob::DOORRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find DOOR EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::DOORRecord* target = reinterpret_cast<Sk::DOORRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to DOOR: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->DOOR.pool.construct(targets.at(i), NULL, false);
	}

	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->DOOR.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
			Sk::DOORRecord* acti = (Sk::DOORRecord*) modRecords.at(i);
			acti->IsChanged(true);
	}
	
}

/*std::vector<Sk::ACHRRecord*> getACHR(std::vector<Record*, std::allocator<Record*>> achrRecords, FORMID nameFromID)//WTM:  Added:  I've added this as part of an attempt to attach scripts to ACHR instead of NPC.
{
	std::vector<Sk::ACHRRecord*> matchingACHRRecords=std::vector<Sk::ACHRRecord*>();
	for (int i = 0; i < achrRecords.size(); ++i)
	{
		Sk::ACHRRecord* achr = (Sk::ACHRRecord*)achrRecords[i];
		if (achr->NAME.value == nameFromID)
		{
			matchingACHRRecords.push_back(achr);
		}
	}
	return matchingACHRRecords;
}*/

void convertNPC_(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> Creatures;
	std::vector<Record*, std::allocator<Record*>> LeveledCrea;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->NPC_.pool.MakeRecordsVector(obRecords);
	oblivionFile->CREA.pool.MakeRecordsVector(Creatures);
	oblivionFile->LVLC.pool.MakeRecordsVector(LeveledCrea);
	skyblivionFile->NPC_.pool.MakeRecordsVector(skbRecords);
	geckFile->NPC_.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::NPC_Record*> npcTargets = std::vector<Sk::NPC_Record*>();

	//WTM:  Note:  Creation Kit logs errors like this:  TES4MQ06MythicDawnAnteGuardF03 (01094E81) cannot be scripted, but has scripts attached to it.
	//This error seems to only occur for NPC_ and CREA in conjunction with LVLC.
	//The below commented sections are my attempt at moving the VMAD record from NPC_s and CREAs to the references that utilize the NPC_s and CREAs.
	//This resulted in tons of errors in SSEEdit.
	/*std::vector<Record*, std::allocator<Record*>> skyblivionACHRRecords;
	skyblivionFile->CELL.achr_pool.MakeRecordsVector(skyblivionACHRRecords);

	std::vector<Sk::WRLDRecord*> wrldTargets = std::vector<Sk::WRLDRecord*>();
	std::vector<Sk::CELLRecord*> cellTargets = std::vector<Sk::CELLRecord*>();
	std::vector<Sk::ACHRRecord*> achrTargets = std::vector<Sk::ACHRRecord*>();*/
	log_debug << obRecords.size() << " NPCs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::NPC_Record *p = (Ob::NPC_Record*)obRecords[it];


		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find NPC_ EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}

			Sk::NPC_Record* target = reinterpret_cast<Sk::NPC_Record*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			//std::vector<Sk::ACHRRecord*> matchingACHRRecords = getACHR(skyblivionACHRRecords, target->formID);//WTM:  Change:  Added

			try
			{
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);
				/*if (matchingACHRRecords.size() > 0)
				{
					for (int i = 0; i < matchingACHRRecords.size(); ++i)
					{
						Sk::ACHRRecord* achr = matchingACHRRecords[i];
						if (!achr->VMAD.IsLoaded()) { achr->VMAD.Load(); }
						if (achr->VMAD.value->scripts.size() < 1) { achr->VMAD.value = new VMADRecord(); }// Do not override if there are already scripts in VMAD record
						achr->VMAD.value->scripts.push_back(convertedScript);
						achr->IsChanged(true); //Hack - idk why it doesn't mark itself..
						achrTargets.push_back(achr);
						Sk::CELLRecord* cell = (Sk::CELLRecord*)achr->GetParentRecord();
						bool cellExists = false; for (int c = 0; c < cellTargets.size(); c++) { if (cellTargets[c]->formID == cell->formID) { cellExists = true; } }
						if (!cellExists || true) { cellTargets.push_back(cell); }
						Sk::WRLDRecord* wrld = (Sk::WRLDRecord*)cell->GetParentRecord();
						if (wrld != NULL)
						{
							bool wrldExists = false; for (int w = 0; w < wrldTargets.size(); w++) { if (wrldTargets[w]->formID == wrld->formID) { wrldExists = true; } }
							if (!wrldExists) { wrldTargets.push_back(wrld); }
						}
						log_debug << "Moved NPC_ " << target->formID << " VMAD to ACHR " << achr->formID << " in CELL " << cell->formID << std::endl;
					}
				}
				else
				{*/
					target->VMAD = VMADRecord();
					target->VMAD.scripts.push_back(convertedScript);
					target->IsChanged(true); //Hack - idk why it doesn't mark itself..
					npcTargets.push_back(target);
				//}
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to NPC_: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}

	log_debug << Creatures.size() << " CREAs found in oblivion file.\n";
	for (uint32_t it = 0; it < Creatures.size(); ++it) {
		Ob::CREARecord *p = (Ob::CREARecord*)Creatures[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find NPC_ ( old CREA ) EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::NPC_Record* target = reinterpret_cast<Sk::NPC_Record*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			//std::vector<Sk::ACHRRecord*> matchingACHRRecords = getACHR(skyblivionACHRRecords, target->formID);//WTM:  Change:  Added

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);
				/*if (matchingACHRRecords.size() > 0)
				{
					for (int i = 0; i < matchingACHRRecords.size(); ++i)
					{
						Sk::ACHRRecord* achr = matchingACHRRecords[i];
						if (!achr->VMAD.IsLoaded()) { achr->VMAD.Load(); }
						if (achr->VMAD.value->scripts.size() < 1) { achr->VMAD.value = new VMADRecord(); }// Do not override if there are already scripts in VMAD record
						achr->VMAD.value->scripts.push_back(convertedScript);
						achr->IsChanged(true); //Hack - idk why it doesn't mark itself..
						achrTargets.push_back(achr);
						Sk::CELLRecord* cell = (Sk::CELLRecord*)achr->GetParentRecord();
						bool cellExists = false; for (int c = 0; c < cellTargets.size(); c++) { if (cellTargets[c]->formID == cell->formID) { cellExists = true; } }
						if (!cellExists || true) { cellTargets.push_back(cell); }
						Sk::WRLDRecord* wrld = (Sk::WRLDRecord*)cell->GetParentRecord();
						if (wrld != NULL)
						{
							bool wrldExists = false; for (int w = 0; w < wrldTargets.size(); w++) { if (wrldTargets[w]->formID == wrld->formID) { wrldExists = true; } }
							if (!wrldExists) { wrldTargets.push_back(wrld); }
						}
						log_debug << "Moved CREA " << target->formID << " VMAD to ACHR " << achr->formID << " in CELL " << cell->formID << std::endl;
					}
				}
				else
				{*/
					target->VMAD = VMADRecord();
					target->VMAD.scripts.push_back(convertedScript);
					target->IsChanged(true); //Hack - idk why it doesn't mark itself..
					npcTargets.push_back(target);
				//}
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to NPC_: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}

	log_debug << LeveledCrea.size() << " LVLCs found in oblivion file.\n";
	for (uint32_t it = 0; it < LeveledCrea.size(); ++it) {
		Ob::LVLCRecord *p = (Ob::LVLCRecord*)LeveledCrea[it];
		if (p->SCRI.IsLoaded()) {
			std::string lvlnEdid = "TES4" + std::string(p->EDID.value);
			std::transform(lvlnEdid.begin(), lvlnEdid.end(), lvlnEdid.begin(), ::tolower);
			FORMID lvlnFormid = converter.findRecordFormidByEDID(lvlnEdid);
			if (lvlnFormid == NULL) {
				log_error << "Cannot find LVLN  EDID " << lvlnEdid << std::endl;
				continue;
			}

			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) { return (lvlnFormid & 0x00FFFFFF) == (((Sk::NPC_Record*)record)->TPLT.value & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_warning << "Cannot find NPC_, LVLN EDID " << lvlnEdid << " LVLN formid (NPC_->TPLT) " << lvlnFormid << std::endl;
				continue;
			}
			Sk::NPC_Record* target = reinterpret_cast<Sk::NPC_Record*>(*foundRec);
			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			//std::vector<Sk::ACHRRecord*> matchingACHRRecords = getACHR(skyblivionACHRRecords, target->formID);//WTM:  Change:  Added

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);
				/*if (matchingACHRRecords.size() > 0)
				{
					for (int i = 0; i < matchingACHRRecords.size(); ++i)
					{
						Sk::ACHRRecord* achr = matchingACHRRecords[i];
						if (!achr->VMAD.IsLoaded()) { achr->VMAD.Load(); }
						if (achr->VMAD.value->scripts.size() < 1) { achr->VMAD.value = new VMADRecord(); }// Do not override if there are already scripts in VMAD record
						achr->VMAD.value->scripts.push_back(convertedScript);
						achr->IsChanged(true); //Hack - idk why it doesn't mark itself..
						achrTargets.push_back(achr);
						Sk::CELLRecord* cell = (Sk::CELLRecord*)achr->GetParentRecord();
						bool cellExists = false; for (int c = 0; c < cellTargets.size(); c++) { if (cellTargets[c]->formID == cell->formID) { cellExists = true; } }
						if (!cellExists || true) { cellTargets.push_back(cell); }
						Sk::WRLDRecord* wrld = (Sk::WRLDRecord*)cell->GetParentRecord();
						if (wrld != NULL)
						{
							bool wrldExists = false; for (int w = 0; w < wrldTargets.size(); w++) { if (wrldTargets[w]->formID == wrld->formID) { wrldExists = true; } }
							if (!wrldExists) { wrldTargets.push_back(wrld); }
						}
						log_debug << "Moved LVLC " << target->formID << " VMAD to ACHR " << achr->formID << " in CELL " << cell->formID << std::endl;
					}
				}
				else
				{*/
					// Do not override if there are already scripts in VMAD record
					if (target->VMAD.scripts.size() < 1)
						target->VMAD = VMADRecord();

					target->VMAD.scripts.push_back(convertedScript);
					target->IsChanged(true); //Hack - idk why it doesn't mark itself..
					npcTargets.push_back(target);
				//}
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to NPC_: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}
		}

	}


	for (uint32_t i = 0; i < npcTargets.size(); i++) {
		geckFile->NPC_.pool.construct(npcTargets.at(i), NULL, false);
	}
	/*for (uint32_t i = 0; i < wrldTargets.size(); i++) {
		Sk::WRLDRecord* wrld = wrldTargets.at(i);
		Sk::WRLDRecord* newWRLD = (Sk::WRLDRecord*)geckFile->WRLD.wrld_pool.construct(wrld, NULL, false);
		newWRLD->CELL = wrld->CELL;
	}
	for (uint32_t i = 0; i < cellTargets.size(); i++) {
		Sk::CELLRecord* cell = cellTargets.at(i);
		Sk::CELLRecord* newCELL = (Sk::CELLRecord*)geckFile->CELL.cell_pool.construct(cellTargets.at(i), NULL, false);
		newCELL->ACHR = cell->ACHR;
	}
	for (uint32_t i = 0; i < achrTargets.size(); i++) {
		geckFile->CELL.achr_pool.construct(achrTargets.at(i), NULL, false);
	}*/

	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->NPC_.pool.MakeRecordsVector(modRecords);
	for (uint32_t i = 0; i < modRecords.size(); i++) {
			Sk::NPC_Record* acti = (Sk::NPC_Record*) modRecords.at(i);
			acti->IsChanged(true);
	}
	
	/*modRecords.clear();
	geckFile->CELL.achr_pool.MakeRecordsVector(modRecords);
	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::ACHRRecord* achr = (Sk::ACHRRecord*) modRecords.at(i);
		achr->IsChanged(true);
	}

	modRecords.clear();
	geckFile->CELL.cell_pool.MakeRecordsVector(modRecords);
	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::CELLRecord* cell = (Sk::CELLRecord*) modRecords.at(i);
		cell->IsChanged(true);
	}*/
}

void convertWEAP(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->WEAP.pool.MakeRecordsVector(obRecords);
	skyblivionFile->WEAP.pool.MakeRecordsVector(skbRecords);
	geckFile->WEAP.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::WEAPRecord*> targets = std::vector<Sk::WEAPRecord*>();
	log_debug << obRecords.size() << " WEAPs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::WEAPRecord *p = (Ob::WEAPRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find WEAP EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::WEAPRecord* target = reinterpret_cast<Sk::WEAPRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to WEAP: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->WEAP.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->WEAP.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
			Sk::WEAPRecord* acti = (Sk::WEAPRecord*) modRecords.at(i);
			acti->IsChanged(true);
	}

}

void convertARMO(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> obClotRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->ARMO.pool.MakeRecordsVector(obRecords);
	oblivionFile->CLOT.pool.MakeRecordsVector(obClotRecords);
	skyblivionFile->ARMO.pool.MakeRecordsVector(skbRecords);
	geckFile->ARMO.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::ARMORecord*> targets = std::vector<Sk::ARMORecord*>();
	log_debug << obRecords.size() << " ARMOs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::ARMORecord *p = (Ob::ARMORecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find ARMO EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::ARMORecord* target = reinterpret_cast<Sk::ARMORecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to ARMO: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}

	log_debug << obRecords.size() << " CLOTs found in oblivion file.\n";
	for (uint32_t it = 0; it < obClotRecords.size(); ++it) {
		Ob::CLOTRecord *p = (Ob::CLOTRecord*)obClotRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find ARMO (old CLOT) EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::ARMORecord* target = reinterpret_cast<Sk::ARMORecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to ARMO: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->ARMO.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->ARMO.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::ARMORecord* acti = (Sk::ARMORecord*) modRecords.at(i);
		acti->IsChanged(true);
	}

}

void convertBOOK(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->BOOK.pool.MakeRecordsVector(obRecords);
	skyblivionFile->BOOK.pool.MakeRecordsVector(skbRecords);
	geckFile->BOOK.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::BOOKRecord*> targets = std::vector<Sk::BOOKRecord*>();
	log_debug << obRecords.size() << " BOOKs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::BOOKRecord *p = (Ob::BOOKRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find BOOK EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::BOOKRecord* target = reinterpret_cast<Sk::BOOKRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to BOOK: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->BOOK.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->BOOK.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::BOOKRecord* acti = (Sk::BOOKRecord*) modRecords.at(i);
		acti->IsChanged(true);
	}

}

void convertINGR(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->INGR.pool.MakeRecordsVector(obRecords);
	skyblivionFile->INGR.pool.MakeRecordsVector(skbRecords);
	geckFile->INGR.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::INGRRecord*> targets = std::vector<Sk::INGRRecord*>();
	log_debug << obRecords.size() << " INGRs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::INGRRecord *p = (Ob::INGRRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find INGR EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::INGRRecord* target = reinterpret_cast<Sk::INGRRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				//target->VMAD = OptSubRecord<VMADRecord>();
				//target->VMAD.Load();
				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				//target->VMAD.value->scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to INGR: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->INGR.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->INGR.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::INGRRecord* acti = (Sk::INGRRecord*) modRecords.at(i);
		acti->IsChanged(true);
	}

}

void convertKEYM(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->KEYM.pool.MakeRecordsVector(obRecords);
	skyblivionFile->KEYM.pool.MakeRecordsVector(skbRecords);
	geckFile->KEYM.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::KEYMRecord*> targets = std::vector<Sk::KEYMRecord*>();
	log_debug << obRecords.size() << " KEYMs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::KEYMRecord *p = (Ob::KEYMRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find KEYM EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::KEYMRecord* target = reinterpret_cast<Sk::KEYMRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to KEYM: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->KEYM.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->KEYM.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::KEYMRecord* acti = (Sk::KEYMRecord*) modRecords.at(i);
		acti->IsChanged(true);
	}

}

void convertMISC(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> obSgstRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->MISC.pool.MakeRecordsVector(obRecords);
	oblivionFile->SGST.pool.MakeRecordsVector(obSgstRecords);
	skyblivionFile->MISC.pool.MakeRecordsVector(skbRecords);
	geckFile->MISC.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::MISCRecord*> targets = std::vector<Sk::MISCRecord*>();
	log_debug << obRecords.size() << " MISCs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::MISCRecord *p = (Ob::MISCRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find MISC EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::MISCRecord* target = reinterpret_cast<Sk::MISCRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to MISC: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}

	printer("%d SGSTs found in oblivion file.\n", obSgstRecords.size());
	for (uint32_t it = 0; it < obSgstRecords.size(); ++it) {
		Ob::SGSTRecord *p = (Ob::SGSTRecord*)obSgstRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find MISC (old SGST) EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::MISCRecord* target = reinterpret_cast<Sk::MISCRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to MISC: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->MISC.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->MISC.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::MISCRecord* acti = (Sk::MISCRecord*) modRecords.at(i);
		acti->IsChanged(true);
	}

}

void convertFLOR(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->FLOR.pool.MakeRecordsVector(obRecords);
	skyblivionFile->FLOR.pool.MakeRecordsVector(skbRecords);
	geckFile->FLOR.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::FLORRecord*> targets = std::vector<Sk::FLORRecord*>();
	log_debug << obRecords.size() << " FLORs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::FLORRecord *p = (Ob::FLORRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find FLOR EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::FLORRecord* target = reinterpret_cast<Sk::FLORRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to FLOR: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->FLOR.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->FLOR.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::FLORRecord* acti = (Sk::FLORRecord*) modRecords.at(i);
		acti->IsChanged(true);
	}

}

void convertFURN(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->FURN.pool.MakeRecordsVector(obRecords);
	skyblivionFile->FURN.pool.MakeRecordsVector(skbRecords);
	geckFile->FURN.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::FURNRecord*> targets = std::vector<Sk::FURNRecord*>();
	log_debug << obRecords.size() << " FURNs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::FURNRecord *p = (Ob::FURNRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find FURN EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::FURNRecord* target = reinterpret_cast<Sk::FURNRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to FURN: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->FURN.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->FURN.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::FURNRecord* acti = (Sk::FURNRecord*) modRecords.at(i);
		acti->IsChanged(true);
	}

}

void convertLIGH(SkyblivionConverter &converter) {
	TES4File* oblivionFile = converter.getOblivionFile();
	TES5File* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> obRecords;
	std::vector<Record*, std::allocator<Record*>> skbRecords;
	std::vector<Record*, std::allocator<Record*>> modRecords;
	oblivionFile->LIGH.pool.MakeRecordsVector(obRecords);
	skyblivionFile->LIGH.pool.MakeRecordsVector(skbRecords);
	geckFile->LIGH.pool.MakeRecordsVector(skbRecords);
	std::vector<Sk::LIGHRecord*> targets = std::vector<Sk::LIGHRecord*>();
	log_debug << obRecords.size() << " LIGHs found in oblivion file.\n";
	for (uint32_t it = 0; it < obRecords.size(); ++it) {
		Ob::LIGHRecord *p = (Ob::LIGHRecord*)obRecords[it];

		if (p->SCRI.IsLoaded()) {
			auto foundRec = std::find_if(skbRecords.begin(), skbRecords.end(), [=](const Record* record) {return (p->formID & 0x00FFFFFF) == (record->formID & 0x00FFFFFF); });
			if (foundRec == skbRecords.end())
			{
				log_error << "Cannot find LIGH EDID " << std::string(p->GetEditorIDKey()) << std::endl;
				continue;
			}
			Sk::LIGHRecord* target = reinterpret_cast<Sk::LIGHRecord*>(*foundRec);

			//Find the script
			Ob::SCPTRecord* script = reinterpret_cast<Ob::SCPTRecord*>(*std::find_if(converter.getScripts().begin(), converter.getScripts().end(), [=](const Record* record) { return record->formID == p->SCRI.value;  }));

			try {
				Script* convertedScript = converter.createVirtualMachineScriptFor(script);

				target->VMAD = VMADRecord();
				target->VMAD.scripts.push_back(convertedScript);
				target->IsChanged(true); //Hack - idk why it doesn't mark itself..
				targets.push_back(target);
			}
			catch (std::exception &ex) {
				log_error << "Cannot bind script to LIGH: " + std::string(ex.what()) << std::endl;
				continue; //Cannot find - thats fine
			}

		}

	}


	for (uint32_t i = 0; i < targets.size(); i++) {
		geckFile->LIGH.pool.construct(targets.at(i), NULL, false);
	}


	//TODO:
	//a) IsChanged flag should be passed on in copy constructor
	//b) It should be automatically marked when changing fields ( requires encapsulation of input to records )
	geckFile->LIGH.pool.MakeRecordsVector(modRecords);

	for (uint32_t i = 0; i < modRecords.size(); i++) {
		Sk::LIGHRecord* acti = (Sk::LIGHRecord*) modRecords.at(i);
		acti->IsChanged(true);
	}

}

void addSpeakAsNpcs(SkyblivionConverter &converter, Collection &skyrimCollection) {
	std::string metadataFile = converter.ROOT_BUILD_PATH() + "Metadata.txt";//WTM:  Change:  Added .txt
	std::FILE* scriptHandle = std::fopen(metadataFile.c_str(), "r");
	if (!scriptHandle) {
		log_error << "Couldn't find Metadata File\n";
		return;
	}

	fseek(scriptHandle, 0, SEEK_END);
	size_t size = ftell(scriptHandle);
	char* scriptData = new char[size];
	rewind(scriptHandle);
	fread(scriptData, sizeof(char), size, scriptHandle);
	fclose(scriptHandle);
	std::string fullScript(scriptData);
	delete[] scriptData;

	boost::regex propRegex("ADD_SPEAK_AS_ACTOR (.*?)\\n");

	boost::sregex_iterator properties(fullScript.begin(), fullScript.end(), propRegex, boost::match_not_dot_newline);
	boost::sregex_iterator end;

	std::string colPrefix = "col_";

	ModFile* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	
	Sk::CELLRecord *newCell = new Sk::CELLRecord();
	newCell->formID = skyrimCollection.NextFreeExpandedFormID(skyblivionFile);

	std::string cellEdid = std::string();
	cellEdid = "TES4SpeakAsHoldingCell";
	char *cstr = new char[cellEdid.length() + 1];
	strncpy(cstr, cellEdid.c_str(), cellEdid.length() + 1);
	newCell->EDID.value = cstr;

	std::vector<std::string> actorNames;

	for (; properties != end; ++properties) {
		std::string actorName = (*properties)[1];

		if (std::find(actorNames.begin(), actorNames.end(), actorName) != actorNames.end())
			continue;

		actorNames.push_back(actorName);

		std::string achrEdid = "TES4" + actorName + "Ref";
		cstr = new char[achrEdid.length() + 1];
		strncpy(cstr, achrEdid.c_str(), achrEdid.length() + 1);
		std::transform(achrEdid.begin(), achrEdid.end(), achrEdid.begin(), ::tolower);

		if (converter.findRecordFormidByEDID(achrEdid) != NULL) {
			log_info << achrEdid << " already exists, new ACHR record won't be created\n";
			continue;
		}

		Sk::ACHRRecord *newAchr = new Sk::ACHRRecord();
		newAchr->formID = skyrimCollection.NextFreeExpandedFormID(skyblivionFile);

		newAchr->EDID.value = cstr;
		newAchr->flags = 0x400;

		std::transform(actorName.begin(), actorName.end(), actorName.begin(), ::tolower);
		FORMID actorFormid = converter.findRecordFormidByEDID("tes4" + actorName);

		if (actorFormid == NULL) {
			log_error << "Couldn't find FORMID for the actor tes4" << actorName << "\n";
			continue;
		}

		newAchr->NAME.value = actorFormid;
		GENPOSDATA *achrPos = new GENPOSDATA();
		achrPos->posX = 0;
		achrPos->posY = 0;
		achrPos->posZ = 0;
		achrPos->rotX = 0;
		achrPos->rotY = 0;
		achrPos->rotZ = 0;
		newAchr->DATA.value = achrPos;
		newCell->ACHR.push_back(newAchr);

		converter.insertToEdidMap(achrEdid, newAchr->formID);
	}

	geckFile->CELL.cell_pool.construct(newCell, NULL, false);

	std::transform(cellEdid.begin(), cellEdid.end(), cellEdid.begin(), ::tolower);

	converter.insertToEdidMap(cellEdid, newCell->formID);
}

Sk::PACKRecord* getLockPackageTemplate(Sk::PACKRecord* src) {
    Sk::PACKRecord* copy = new Sk::PACKRecord(*src);

    /*
    *   Add TDAT entries
    */
    uint8_t newPData = copy->XNAM.value;
    char* actBool = "Bool";
    Sk::PACKRecord::PACKACTIVITY p;

    // Lock at Start?
    copy->TDAT.unamData.push_back(newPData++);
    copy->TDAT.ANAM.push_back(actBool);
    p = Sk::PACKRecord::PACKACTIVITY();
    p.writtenBool = false;
    copy->TDAT.cnamData.push_back(p);

    // Lock at Location?
    copy->TDAT.unamData.push_back(newPData++);
    copy->TDAT.ANAM.push_back(actBool);
    p = Sk::PACKRecord::PACKACTIVITY();
    p.writtenBool = false;
    copy->TDAT.cnamData.push_back(p);

    // Lock at End?
    copy->TDAT.unamData.push_back(newPData++);
    copy->TDAT.ANAM.push_back(actBool);
    p = Sk::PACKRecord::PACKACTIVITY();
    p.writtenBool = false;
    copy->TDAT.cnamData.push_back(p);

    // Unlock at Start?
    copy->TDAT.unamData.push_back(newPData++);
    copy->TDAT.ANAM.push_back(actBool);
    p = Sk::PACKRecord::PACKACTIVITY();
    p.writtenBool = false;
    copy->TDAT.cnamData.push_back(p);

    // Unlock at Location?
    copy->TDAT.unamData.push_back(newPData++);
    copy->TDAT.ANAM.push_back(actBool);
    p = Sk::PACKRecord::PACKACTIVITY();
    p.writtenBool = false;
    copy->TDAT.cnamData.push_back(p);

    // Unlock at End?
    copy->TDAT.unamData.push_back(newPData++);
    copy->TDAT.ANAM.push_back(actBool);
    p = Sk::PACKRecord::PACKACTIVITY();
    p.writtenBool = false;
    copy->TDAT.cnamData.push_back(p);

    // Near self (location)
    copy->TDAT.unamData.push_back(newPData++);
    char* actLoc = "Location";
    copy->TDAT.ANAM.push_back(actLoc);
    p = Sk::PACKRecord::PACKACTIVITY();
    p.writtenPLDT.locType = 12;
    p.writtenPLDT.locRadius = 1000;
    copy->TDAT.cnamData.push_back(p);

    // Update public package data count
    copy->PKCU.value.dataInputCount += newPData - copy->XNAM.value;
    // Update XNAM value
    copy->XNAM.value = newPData;

    /*
    *   Add procedures to the tree
    */
    // Lock at Start
    Sk::PACKRecord::PACKPTRE* ptre = new Sk::PACKRecord::PACKPTRE();
    ptre->ANAM = "Procedure";
    ptre->PNAM = "LockDoors";
    ptre->FNAM = 0;
    ptre->PKC2.push_back(newPData - 1); // Location: Near Self, 512

    ptre->CITC = 1;
    Sk::SKCondition* cond = new Sk::SKCondition();
    cond->CTDA.value.compValue = 0x3F800000; // 1
    cond->CTDA.value.ifunc = 612;
    cond->CTDA.value.param1 = newPData - 7; // Lock at Start?
    ptre->CTDA.value.push_back(cond);

    copy->PTRE.value[0]->PRCB.numOfChild++; // Increase main branch's child count
    copy->PTRE.value.insert(copy->PTRE.value.begin() + 1, ptre); // Add to 2. place

    // Lock at Location
    // Find first main branch Travel procedure
    for (int i = 2; i < copy->PTRE.value.size(); i++) {
        if (std::string(copy->PTRE.value[i]->PNAM) == "Travel") {
            ptre = new Sk::PACKRecord::PACKPTRE();
            ptre->ANAM = "Procedure";
            ptre->PNAM = "LockDoors";
            ptre->FNAM = 0;
            ptre->PKC2.push_back(newPData - 1); // Location: Near Self, 512

            ptre->CITC = 1;
            cond = new Sk::SKCondition();
            cond->CTDA.value.compValue = 0x3F800000; // 1
            cond->CTDA.value.ifunc = 612;
            cond->CTDA.value.param1 = newPData - 6; // Lock at Location?
            ptre->CTDA.value.push_back(cond);

            copy->PTRE.value[0]->PRCB.numOfChild++; // Increase main branch's child count
            copy->PTRE.value.insert(copy->PTRE.value.begin() + i + 1, ptre); // Add after Travel
            break;
        }
        else {
            // if a child branch, skip it
            if (std::string(copy->PTRE.value[i]->ANAM) != "Procedure") {
                i += copy->PTRE.value[i]->PRCB.numOfChild;
            }
        }
    }

    // Lock at End
    ptre = new Sk::PACKRecord::PACKPTRE();
    ptre->ANAM = "Procedure";
    ptre->PNAM = "LockDoors";
    ptre->FNAM = 1;
    ptre->PKC2.push_back(newPData - 1); // Location: Near Self, 512

    ptre->CITC = 1;
    cond = new Sk::SKCondition();
    cond->CTDA.value.compValue = 0x3F800000; // 1
    cond->CTDA.value.ifunc = 612;
    cond->CTDA.value.param1 = newPData - 5; // Lock at Start?
    ptre->CTDA.value.push_back(cond);

    copy->PTRE.value[0]->PRCB.numOfChild++; // Increase main branch's child count
    copy->PTRE.value.push_back(ptre); // Add to the end

    // Unlock at Start
    ptre = new Sk::PACKRecord::PACKPTRE();
    ptre->ANAM = "Procedure";
    ptre->PNAM = "UnlockDoors";
    ptre->FNAM = 0;
    ptre->PKC2.push_back(newPData - 1); // Location: Near Self, 512

    ptre->CITC = 1;
    cond = new Sk::SKCondition();
    cond->CTDA.value.compValue = 0x3F800000; // 1
    cond->CTDA.value.ifunc = 612;
    cond->CTDA.value.param1 = newPData - 4; // Unlock at Start?
    ptre->CTDA.value.push_back(cond);

    copy->PTRE.value[0]->PRCB.numOfChild++; // Increase main branch's child count
    copy->PTRE.value.insert(copy->PTRE.value.begin() + 1, ptre); // Add to 2. place


    // Unlock at Location
    // Find first main branch Travel procedure
    for (int i = 2; i < copy->PTRE.value.size(); i++) {
        if (std::string(copy->PTRE.value[i]->PNAM) == "Travel") {
            ptre = new Sk::PACKRecord::PACKPTRE();
            ptre->ANAM = "Procedure";
            ptre->PNAM = "UnlockDoors";
            ptre->FNAM = 0;
            ptre->PKC2.push_back(newPData - 1); // Location: Near Self, 512

            ptre->CITC = 1;
            cond = new Sk::SKCondition();
            cond->CTDA.value.compValue = 0x3F800000; // 1
            cond->CTDA.value.ifunc = 612;
            cond->CTDA.value.param1 = newPData - 3; // Unlock at Location?
            ptre->CTDA.value.push_back(cond);

            copy->PTRE.value[0]->PRCB.numOfChild++; // Increase main branch's child count
            copy->PTRE.value.insert(copy->PTRE.value.begin() + i + 1, ptre); // Add after Travel
            break;
        }
        else {
            // if a child branch, skip it
            if (std::string(copy->PTRE.value[i]->ANAM) != "Procedure") {
                i += copy->PTRE.value[i]->PRCB.numOfChild;
            }
        }
    }

    // Unlock at End
    ptre = new Sk::PACKRecord::PACKPTRE();
    ptre->ANAM = "Procedure";
    ptre->PNAM = "UnlockDoors";
    ptre->FNAM = 1;
    ptre->PKC2.push_back(newPData - 1); // Location: Near Self, 512

    ptre->CITC = 1;
    cond = new Sk::SKCondition();
    cond->CTDA.value.compValue = 0x3F800000; // 1
    cond->CTDA.value.ifunc = 612;
    cond->CTDA.value.param1 = newPData - 2; // Unlock at Start?
    ptre->CTDA.value.push_back(cond);

    copy->PTRE.value[0]->PRCB.numOfChild++; // Increase main branch's child count
    copy->PTRE.value.push_back(ptre); // Add to the end

    /*
    *   Add package data infos
    */
    Sk::PACKRecord::PACKPDAT* pdat;

    pdat = new Sk::PACKRecord::PACKPDAT();
    pdat->UNAM = newPData - 7;
    pdat->PNAM = 1;
    pdat->BNAM = "Lock at Start?";
    copy->PDAT.value.push_back(pdat);

    pdat = new Sk::PACKRecord::PACKPDAT();
    pdat->UNAM = newPData - 6;
    pdat->PNAM = 1;
    pdat->BNAM = "Lock at Location?";
    copy->PDAT.value.push_back(pdat);

    pdat = new Sk::PACKRecord::PACKPDAT();
    pdat->UNAM = newPData - 5;
    pdat->PNAM = 1;
    pdat->BNAM = "Lock at End?";
    copy->PDAT.value.push_back(pdat);

    pdat = new Sk::PACKRecord::PACKPDAT();
    pdat->UNAM = newPData - 4;
    pdat->PNAM = 1;
    pdat->BNAM = "Unlock at Start?";
    copy->PDAT.value.push_back(pdat);

    pdat = new Sk::PACKRecord::PACKPDAT();
    pdat->UNAM = newPData - 3;
    pdat->PNAM = 1;
    pdat->BNAM = "Unlock at Location?";
    copy->PDAT.value.push_back(pdat);

    pdat = new Sk::PACKRecord::PACKPDAT();
    pdat->UNAM = newPData - 2;
    pdat->PNAM = 1;
    pdat->BNAM = "Unlock at End?";
    copy->PDAT.value.push_back(pdat);

    pdat = new Sk::PACKRecord::PACKPDAT();
    pdat->UNAM = newPData - 1;
    pdat->PNAM = 1;
    pdat->BNAM = "Near Self";
    copy->PDAT.value.push_back(pdat);

    return copy;
}

void addPackageTemplates(SkyblivionConverter &converter, Collection &skyrimCollection) {
	TES5File* skyrimFile = converter.getSkyrimFile();
	ModFile* skyblivionFile = converter.getSkyblivionFile();
	TES5File* geckFile = converter.getGeckFile();
	std::vector<Record*, std::allocator<Record*>> skyrimPacks;
	skyrimFile->PACK.pool.MakeRecordsVector(skyrimPacks);
    std::vector<Record*, std::allocator<Record*>> skbPacks;
    ((TES5File*)skyblivionFile)->PACK.pool.MakeRecordsVector(skbPacks);


    for (int i = 0; i < skbPacks.size(); i++) {
        Sk::PACKRecord* pack = (Sk::PACKRecord*)skbPacks[i];
        std::string edid = std::string(pack->EDID.IsLoaded() ? pack->EDID.value : "");

        if (edid == "TES4FindPackageTemplate") {
            Sk::PACKRecord* copy = getLockPackageTemplate(pack);

            copy->EDID.value = "TES4FindLockPackageTemplate";
            copy->formID = skyrimCollection.NextFreeExpandedFormID(skyblivionFile);

            converter.insertToEdidMap("TES4FindLockPackageTemplate", copy->formID);
            geckFile->PACK.pool.construct(copy, NULL, false);
        }

        if (edid == "TES4UseItemAtPackageTemplate") {
            Sk::PACKRecord* copy = getLockPackageTemplate(pack);

            copy->EDID.value = "TES4UseItemAtLockPackageTemplate";
            copy->formID = skyrimCollection.NextFreeExpandedFormID(skyblivionFile);

            converter.insertToEdidMap("TES4UseItemAtLockPackageTemplate", copy->formID);
            geckFile->PACK.pool.construct(copy, NULL, false);
        }
    }

	for (int i = 0; i < skyrimPacks.size(); i++) {
		Sk::PACKRecord* pack = (Sk::PACKRecord*)skyrimPacks[i];
		std::string edid = std::string(pack->EDID.IsLoaded() ? pack->EDID.value : "");

        if (edid == "Eat") {
            Sk::PACKRecord* copy = getLockPackageTemplate(pack);

            copy->EDID.value = "TES4EatLockTemplate";
            copy->formID = skyrimCollection.NextFreeExpandedFormID(skyblivionFile);

            converter.insertToEdidMap("TES4EatLockTemplate", copy->formID);
            geckFile->PACK.pool.construct(copy, NULL, false);
        }

		if (edid == "Sleep") {
            Sk::PACKRecord* copy = getLockPackageTemplate(pack);

            copy->EDID.value = "TES4SleepLockTemplate";
            copy->formID = skyrimCollection.NextFreeExpandedFormID(skyblivionFile);

            converter.insertToEdidMap("TES4SleepLockTemplate", copy->formID);
            geckFile->PACK.pool.construct(copy, NULL, false);
		}

        if (edid == "Sandbox") {
            Sk::PACKRecord* copy = getLockPackageTemplate(pack);

            copy->EDID.value = "TES4SandboxLockTemplate";
            copy->formID = skyrimCollection.NextFreeExpandedFormID(skyblivionFile);

            converter.insertToEdidMap("TES4SandboxLockTemplate", copy->formID);
            geckFile->PACK.pool.construct(copy, NULL, false);
        }

        if (edid == "Travel") {
            Sk::PACKRecord* copy = getLockPackageTemplate(pack);

            copy->EDID.value = "TES4TravelLockTemplate";
            copy->formID = skyrimCollection.NextFreeExpandedFormID(skyblivionFile);
            copy->PTRE.value[3]->FNAM = 0;

            converter.insertToEdidMap("TES4TravelLockTemplate", copy->formID);
            geckFile->PACK.pool.construct(copy, NULL, false);
        }
	}
}

int main(int argc, char * argv[]) {

	char* input = "Input.esm";
	char* output = "Output.esp";
	char* inputModName = "myMod";

	if (argc < 4) {
		std::cout << "usage: GECKFrontend.exe <input folder> <output folder> <scripts folder>";
		return 0;
	}

	logger.init(argc, argv);

	Collection oblivionCollection = Collection(argv[1], 0);
	Collection skyrimCollection = Collection(argv[2], 3);

	ModFlags obFlags = ModFlags(2);
	TES4File* oblivionMod = (TES4File*)oblivionCollection.AddMod("Oblivion.esm", obFlags);

	ModFlags masterFlags = ModFlags(0xA);
	ModFlags skyblivionFlags = ModFlags(0xA);
	skyrimCollection.AddMod("Skyrim.esm", masterFlags);
	TES5File* skyblivion = (TES5File*)skyrimCollection.AddMod("Skyblivion.esm", skyblivionFlags);

	ModFlags espFlags = ModFlags(0x1818);
	TES5File* skyrimMod = (TES5File*)skyrimCollection.AddMod("GECK.esp", espFlags);
	skyrimMod->TES4.MAST.push_back("Skyrim.esm");
	skyrimMod->TES4.MAST.push_back("Skyblivion.esm");
	skyrimMod->TES4.formVersion = 43;

	log_debug << std::endl << "Loading Oblivion Collection..." << std::endl;
	oblivionCollection.Load();
	log_debug << std::endl << "Oblivion Collection Loaded." << std::endl;
	log_debug << std::endl << "Loading Skyrim Collection..." << std::endl;
	skyrimCollection.Load();

	log_debug << std::endl << "Skyrim Collection Loaded." << std::endl;

	SkyblivionConverter converter = SkyblivionConverter(oblivionCollection, skyrimCollection, std::string(argv[3]));

	log_debug << std::endl << "Converting Speak as NPCs..." << std::endl;
	addSpeakAsNpcs(converter, skyrimCollection);

	log_debug << std::endl << "Converting DIAL records..." << std::endl;
	std::vector<Sk::DIALRecord *> *resDIAL = converter.convertDIALFromOblivion();

	log_debug << std::endl << "Adding SOUN records from SNDR records..." << std::endl;
	converter.addSOUNFromSNDR();

	/**
	* @todo - How we handle topics splitted into N dialogue topics and suffixed by QSTI value?
	*/
	log_debug << std::endl << "Inserting DIAL into EDID Map..." << std::endl;
	for (uint32_t it = 0; it < resDIAL->size(); ++it) {
		Sk::DIALRecord *dial = (Sk::DIALRecord*)(*resDIAL)[it];
		std::string dialEdid = std::string(dial->EDID.value);
		std::transform(dialEdid.begin(), dialEdid.end(), dialEdid.begin(), ::tolower);
		converter.insertToEdidMap(dialEdid, dial->formID);
	}

	log_debug << std::endl << "Converting QUST records..." << std::endl;
	std::vector<Sk::QUSTRecord *> *resQUST = converter.convertQUSTFromOblivion();

	log_debug << std::endl << "Converting PACK records..." << std::endl;
    addPackageTemplates(converter, skyrimCollection);
	converter.convertPACKFromOblivion(oblivionMod, skyrimMod);

	/*
	 * Index new EDIDs and formids
	 */
	log_debug << std::endl << "Inserting QUST into EDID Map..." << std::endl;
	for (uint32_t it = 0; it < resQUST->size(); ++it) {
		Sk::QUSTRecord *qust = (Sk::QUSTRecord*)(*resQUST)[it];
		std::string qustEdid = std::string(qust->EDID.value);
		std::transform(qustEdid.begin(), qustEdid.end(), qustEdid.begin(), ::tolower);
		converter.insertToEdidMap(qustEdid, qust->formID);
	}

	log_debug << std::endl << "Binding properties of INFO and QUST related scripts..." << std::endl;
	converter.bindScriptProperties(resDIAL, resQUST);

	log_debug << std::endl << "Binding VMADs to ACTI records..." << std::endl;
	convertACTI(converter);
	log_debug << std::endl << "Binding VMADs to CONT records..." << std::endl;
	convertCONT(converter);
	log_debug << std::endl << "Binding VMADs to DOOR records..." << std::endl;
	convertDOOR(converter);
	log_debug << std::endl << "Binding VMADs to NPC_ records..." << std::endl;
	convertNPC_(converter);
	log_debug << std::endl << "Binding VMADs to WEAP records..." << std::endl;
	convertWEAP(converter);
	log_debug << std::endl << "Binding VMADs to ARMO records..." << std::endl;
	convertARMO(converter);
	log_debug << std::endl << "Binding VMADs to BOOK records..." << std::endl;
	convertBOOK(converter);
	log_debug << std::endl << "Binding VMADs to INGR records..." << std::endl;
	convertINGR(converter);
	log_debug << std::endl << "Binding VMADs to KEYM records..." << std::endl;
	convertKEYM(converter);
	log_debug << std::endl << "Binding VMADs to MISC records..." << std::endl;
	convertMISC(converter);
	log_debug << std::endl << "Binding VMADs to FLOR records..." << std::endl;
	convertFLOR(converter);
	log_debug << std::endl << "Binding VMADs to FURN records..." << std::endl;
	convertFURN(converter);
	log_debug << std::endl << "Binding VMADs to LIGH records..." << std::endl;
	convertLIGH(converter);

    ModSaveFlags skSaveFlags = ModSaveFlags(2);

	log_debug << std::endl << "Saving..." << std::endl;
    skyrimCollection.SaveMod((ModFile*&)skyrimMod, skSaveFlags, "GECK.esp");
	log_debug << std::endl << "Saved." << std::endl;

    return 0;

}

