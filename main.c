#include "library.h"


#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)






int main(int argc, char** argv) {
	printMenu();
	handler.indexFileName = "org.ind";
	handler.dataFileName = "org.dat";
	handler.MemberFileName = "member.dat";
	handler.dataIndexFile = fopen(handler.indexFileName, "ab");
	handler.MemberDataFile = fopen(handler.MemberFileName, "ab+");
	
	fclose(handler.MemberDataFile);
	if (!doesFileHasContent(handler.dataIndexFile)) {
		int firstIndex = 0;
		fwrite(&firstIndex, sizeof(int), 1, handler.dataIndexFile);
	}
	fclose(handler.dataIndexFile);
	while (handleCommand() != 0) {

	}
	printf("Finishing the prosses");
	return 0;
}


void showOrganizationFile() {
	org organization;
	int i = 1;
	handler.OrganizationDataFile = fopen(handler.dataFileName, "rb");
	if (!doesFileHasContent(handler.OrganizationDataFile))
	{
		printf("There is nothing to show\n");
		return;
	}
	fseek(handler.OrganizationDataFile, 0L, SEEK_SET);
	if (feof(handler.OrganizationDataFile))
	{
		printf("There is nothing to show\n");
		return;
	}
	for (; !feof(handler.OrganizationDataFile);) {
		
		fread(&organization, sizeof(organization), 1, handler.OrganizationDataFile);
		
		printf("%d. %d %d %s %s\n", i, organization.id, organization.budget, organization.address, organization.phone);
		++i;
	}
	fclose(handler.OrganizationDataFile);
}

void insertToIndexFile(org* newOrganization) {
	handler.dataIndexFile = fopen(handler.indexFileName, "rb+");
	int oldIndex;
	fread(&oldIndex, sizeof(int), 1, handler.dataIndexFile);
	newOrganization->id = oldIndex;
	fseek(handler.dataIndexFile, 0L, SEEK_SET);
	int newIndex = oldIndex + 1;
	fwrite(&newIndex, sizeof(int), 1, handler.dataIndexFile);
	fseek(handler.dataIndexFile, 0L, SEEK_END);
	fwrite(&(newOrganization->id), sizeof(int), 1, handler.dataIndexFile);
	fwrite(&oldIndex, sizeof(int), 1, handler.dataIndexFile);
	fclose(handler.dataIndexFile);
}


void insertToMasterFile(char command[MAX_LENGTH_OF_COMMAND]) {
	char delims[] = " \n";
	org newOrganization;
	char* delim = strtok(command, delims);
	
	char* budgetString = strtok(NULL, delims);
	newOrganization.budget = atoi(budgetString);
	strncpy(newOrganization.address, strtok(NULL, delims), Organization_Adress_LEN);
	strncpy(newOrganization.phone, strtok(NULL, delims), PHONE_LEN);
	newOrganization.firstMemberID = -1;
	insertToIndexFile(&newOrganization);

	handler.OrganizationDataFile = fopen(handler.dataFileName, "ab+");
	fwrite(&newOrganization, sizeof(org), 1, handler.OrganizationDataFile);
	fclose(handler.OrganizationDataFile);
}


void printMasterRecord(int index) {
	org hospital;
	if (index == -1) {
		printf("No info with such an index\n");
		return;
	}
	handler.OrganizationDataFile = fopen(handler.dataFileName, "rb+");
	fseek(handler.OrganizationDataFile, index * sizeof(hospital), SEEK_SET);
	fread(&hospital, sizeof(hospital), 1, handler.OrganizationDataFile);
	fclose(handler.OrganizationDataFile);
	printf("ID: %d Budget: %d Address: %s  Phone: %s\n", hospital.id, hospital.budget, hospital.address, hospital.phone);
}


void getRecordFromMasterFile(char command[MAX_LENGTH_OF_COMMAND]) {
	char* delims = " \n";
	char* option = strtok(command, delims);
	int masterID = atoi(strtok(NULL, delims));
	int masterIndex = searchForOrganizationIndex(masterID);

	printMasterRecord(masterIndex);
}


int handleCommand() {
	char* delims = " \n";
	char command[MAX_LENGTH_OF_COMMAND];
	fgets(command, MAX_LENGTH_OF_COMMAND, stdin);
	char copyOfcommand[MAX_LENGTH_OF_COMMAND];
	memcpy(copyOfcommand, command, MAX_LENGTH_OF_COMMAND);
	char* option = strtok(command, delims);
	if (!strcmp(option, "show-m")) {
		showOrganizationFile();
	}
	else if (!strcmp(option, "insert-m")) {
		insertToMasterFile(&copyOfcommand);
	}
	else if (!strcmp(option, "get-m")) {
		getRecordFromMasterFile(&copyOfcommand);
	}
	else if (!strcmp(option, "update-m")) {
		updateOrgFileRecord(&copyOfcommand, delims);
	}
	else if (!strcmp(option, "insert-s")) {
		insertRecordToMemberFile(copyOfcommand, delims);
	}
	else if (!strcmp(option, "get-s")) {
		getRecordFromMemberFile(copyOfcommand, delims);
	}
	else if (!strcmp(option, "update-s")) {
		updateMemberRecord(copyOfcommand, delims);
	}
	else if (!strcmp(option, "delete-m")) {
		deleteOrganizationRecord(copyOfcommand, delims);
	}
	else if (!strcmp(option, "clear")) {
		clearContentOfFiles();
	}
	else if (!strcmp(option, "delete-s")) {
		deleteMemberRecord(copyOfcommand, delims);

	}
	else
		return 0;
	return 1;
}




void printMenu() {

	printf("This is a organization database management programm\n");
	printf("You have next commands available for input:\n");
	printf("1.\"show-m\" - to look information about all organization in database\n");
	
	printf("2.\"get-m ID\" - to look information about organization with ID key\n");
	printf("3.\"get-s ID\" - to look information about all members in organization with ID key\n");
	printf("4.\"get-s organizationID memID\" - to look information about member with memID key in organization with organizationID key\n");
	printf("5.\"insert-m budget address phone\" - to add new organizaton\n");
	printf("6.\"insert-s organizationID name surname birthdate contribution\" - to add new member record with appropriate information to organization with organizationID\n");
	printf("7.\"update-m ID\" - to update record of organization with ID\n");
	printf("8.\"update-s organizationID memID\" - to update record of member with memID in organization with organizationID\n");
	printf("9.\"delete-m organizationID\" - to delete record with organizationID and all their subrecord\n");
	printf("10.\"delete-s organizationID memID\" - to delete record of member with memID\n");
	printf("11.\"clear\" - to erase all the info\n");
	printf("12. or something else - to exit the menu\n");



}

int doesFileHasContent(FILE* f) {
	if (!f)
		return 0;
	fseek(f, 0, SEEK_END);
	unsigned long length = ftell(f);
	if (length != 0) return 1;
	return 0;
}


int searchForOrganizationIndex(int masterID) {

	handler.dataIndexFile = fopen(handler.indexFileName, "rb");
	fseek(handler.dataIndexFile, sizeof(int), SEEK_SET);
	int masterIndex = -1;
	for (;;) {
		int number;
		fread(&number, sizeof(int), 1, handler.dataIndexFile);
		if (feof(handler.dataIndexFile)) break;
		if (number == masterID) {
			fread(&masterIndex, sizeof(int), 1, handler.dataIndexFile);
			break;
		}
		fseek(handler.dataIndexFile, sizeof(int), SEEK_CUR);
	}
	fclose(handler.dataIndexFile);
	return masterIndex;
}

void updateOrgFileRecord(char command[MAX_LENGTH_OF_COMMAND], char* delims) {

	char field[MAX_LEN_OF_FIELD];
	char* delim = strtok(command, delims);
	int id = atoi(strtok(NULL, delims));
	int masterIndex = searchForOrganizationIndex(id);

	org hospital = organizationWithIndex(masterIndex);
	printf("Which field of record would you like to update?\n");
	fgets(field, MAX_LEN_OF_FIELD, stdin);
	deleteEndOfLine(field);
	changeOrganizationRecord(&hospital, field);
	updateRecord(masterIndex, &hospital);
}

org organizationWithIndex(int index) {

	org organization;
	if (index > -1)
	{
		handler.OrganizationDataFile = fopen(handler.dataFileName, "rb+");

		fseek(handler.OrganizationDataFile, index * sizeof(organization), SEEK_SET);
		fread(&organization, sizeof(organization), 1, handler.OrganizationDataFile);
		fclose(handler.OrganizationDataFile);
		return organization;
	}
	else {
		organization.firstMemberID = -1;
		return organization;
	}
}

void changeOrganizationRecord(org* organization, char* field) {

	if (!strcmp(field, "Address")) {
		fgets(organization->address, Organization_Adress_LEN, stdin);
		deleteEndOfLine(organization->address);
	}
	else if (!strcmp(field, "Phone")) {
		fgets(organization->phone, PHONE_LEN, stdin);
		deleteEndOfLine(organization->phone);
	}
	else if (!strcmp(field, "Budget")) {
		scanf("%d", &organization->budget);
		char c = getchar();
	}
	else {
		printf("You can update only existing fields");
	}
}

void updateRecord(int index, org* hospital) {
	handler.OrganizationDataFile = fopen(handler.dataFileName, "rb+");
	fseek(handler.OrganizationDataFile, index * sizeof(*hospital), SEEK_SET);
	fwrite(hospital, sizeof(*hospital), 1, handler.OrganizationDataFile);
	fclose(handler.OrganizationDataFile);
}

void deleteEndOfLine(char* string) {
	char* ptr = string;
	if ((ptr = strchr(string, '\n')) != NULL)
		* ptr = '\0';

}

void getRecordFromMemberFile(char command[MAX_LENGTH_OF_COMMAND], char* delims) {
	char field[MAX_LEN_OF_FIELD];
	char* delim = strtok(command, delims);
	int id = atoi(strtok(NULL, delims));
	char* memIDString = strtok(NULL, delims);
	member mem;
	int index = searchForOrganizationIndex(id);
	org organization = organizationWithIndex(index);

	if (memIDString != NULL) {
		int memID = atoi(memIDString);
		mem = searchInMemberFile(organization.firstMemberID);
		while (mem.id != memID && mem.nextMemberID != -1) {
			mem = searchInMemberFile(mem.nextMemberID);

		}
		printMemberRecord(&mem);
	}
	else {
		if (organization.firstMemberID != -1) {
			mem = searchInMemberFile(organization.firstMemberID);
			while (mem.id != -1) {
				printMemberRecord(&mem);
				mem = searchInMemberFile(mem.nextMemberID);
			}
		}
	}
}
void insertRecordToMemberFile(char command[MAX_LENGTH_OF_COMMAND], char* delims) {

	char* delim = strtok(command, delims);
	member mem;
	int masterID = atoi(strtok(NULL, delims));
	int docID = size_of_MemberFile() + 1;
	mem.id = docID;
	strncpy(mem.name, strtok(NULL, delims), NAME_LEN);
	strncpy(mem.surname, strtok(NULL, delims), SURNAME_LEN);
	strncpy(mem.dateOfBirth, strtok(NULL, delims), LENGTH_OF_BIRTH_DATE);
	int contribution = atoi(strtok(NULL, delims));
	mem.contributions = contribution;
	mem.nextMemberID = -1;
	appendRecordToOrganization(masterID, &mem);
	writeToMemberFile(&mem);
}

void appendAfterLastMemberInOrganization(member* firstMem, member* newMem) {
	while (firstMem->nextMemberID != -1) {
		member mem = (searchInMemberFile(firstMem->nextMemberID));
		firstMem = &mem;
	}
	firstMem->nextMemberID = newMem->id;
	handler.MemberDataFile = fopen(handler.MemberFileName, "rb+");
	for (;;) {
		member oldRecord;
		fread(&oldRecord, sizeof(member), 1, handler.MemberDataFile);
		if (feof(handler.MemberDataFile)) break;
		if (oldRecord.id == firstMem->id) {
			oldRecord.nextMemberID = firstMem->nextMemberID;
			fseek(handler.MemberDataFile, -1L * (int)sizeof(member), SEEK_CUR);
			fwrite(&oldRecord, sizeof(member), 1, handler.MemberDataFile);
			fclose(handler.MemberDataFile);
			break;
		}
	}

}

void appendRecordToOrganization(int orgID, member* mem) {
	int index = searchForOrganizationIndex(orgID);
	org organization = organizationWithIndex(index);
	if (organization.firstMemberID != -1) {
		member firstMember = searchInMemberFile(organization.firstMemberID);
		appendAfterLastMemberInOrganization(&firstMember, mem);
	}
	else {
		organization.firstMemberID = mem->id;
		rewriteOrganizationRecord(organization, index);
	}
}

void rewriteOrganizationRecord(org organ, int index) {
	handler.OrganizationDataFile = fopen(handler.dataFileName, "rb+");
	fseek(handler.OrganizationDataFile, index * sizeof(org), SEEK_SET);
	fwrite(&organ, sizeof(org), 1, handler.OrganizationDataFile);
	fclose(handler.OrganizationDataFile);
}

void writeToMemberFile(member* mem) {

	handler.MemberDataFile = fopen(handler.MemberFileName, "ab+");
	fwrite(mem, sizeof(member), 1, handler.MemberDataFile);
	fclose(handler.MemberDataFile);
}

member searchInMemberFile(int id) {
	member doc;
	handler.MemberDataFile = fopen(handler.MemberFileName, "rb");
	for (;;) {
		fread(&doc, sizeof(member), 1, handler.MemberDataFile);
		if (feof(handler.MemberDataFile)) break;
		if (doc.id == id) {
			fclose(handler.MemberDataFile);
			return doc;
		}
	}
	doc.id = -1;
	fclose(handler.MemberDataFile);
	return doc;
}




void printMemberRecord(member* pMember) {
	printf("ID: %d Name: %s Surname: %s Date Of Birth: %s Contribution: %d\n", pMember->id, pMember->name, pMember->surname, pMember->dateOfBirth, pMember->contributions);
}

void changeMemberRecord(member* pMember, char* field) {
	if (!strcmp(field, "Name")) {
		fgets(pMember->name, NAME_LEN, stdin);
		deleteEndOfLine(pMember->name);
	}
	else if (!strcmp(field, "Surname")) {
		fgets(pMember->surname, SURNAME_LEN, stdin);
		deleteEndOfLine(pMember->surname);
	}
	else if (!strcmp(field, "Contributions")) {
		scanf("%d", &pMember->contributions);
		char c = getchar();
	}
	else if (!strcmp(field, "Birth date")) {
		fgets(pMember->dateOfBirth, LENGTH_OF_BIRTH_DATE, stdin);
		deleteEndOfLine(pMember->dateOfBirth);
	}
	else {
		printf("You can update only existing fields");
	}
}

void updateMemberRecord(char command[MAX_LENGTH_OF_COMMAND], char* delims) {
	char* option = strtok(command, delims);
	int masterID = atoi(strtok(NULL, delims));
	int doctorID = atoi(strtok(NULL, delims));
	printf("Which field of record would you like to update?\n");
	int hospitalIndex = searchForOrganizationIndex(masterID);
	org hosp = organizationWithIndex(hospitalIndex);
	member doc = searchInMemberFile(hosp.firstMemberID);
	while (doc.id != doctorID && doc.id != -1) {
		doc = searchInMemberFile(doc.nextMemberID);
	}
	char field[MAX_LEN_OF_FIELD];
	fgets(field, MAX_LEN_OF_FIELD, stdin);
	deleteEndOfLine(field);
	
	changeMemberRecord(&doc, field);
	handler.MemberDataFile = fopen(handler.MemberFileName, "rb+");
	for (;;) {
		member oldRecord;
		fread(&oldRecord, sizeof(member), 1, handler.MemberDataFile);
		if (feof(handler.MemberDataFile)) break;
		if (oldRecord.id == doc.id) {
			fseek(handler.MemberDataFile, -1L * (int)sizeof(member), SEEK_CUR);
			fwrite(&doc, sizeof(member), 1, handler.MemberDataFile);
			fclose(handler.MemberDataFile);
			break;
		}
	}

}



void deleteOrganizationRecord(char command[MAX_LENGTH_OF_COMMAND], char* delims) {
	char* option = strtok(command, delims);
	int masterID = atoi(strtok(NULL, delims));
	handler.OrganizationDataFile = fopen(handler.dataFileName, "rb");
	FILE* organizationDataTmp = fopen("organization_tmp.dat", "wb");
	org toDelete;
	int found = 0;
	int firstMemId = -1;
	for (;;) {
		fread(&toDelete, sizeof(org), 1, handler.OrganizationDataFile);
		if (feof(handler.OrganizationDataFile)) break;
		if (toDelete.id == masterID) {
			printf("Record found and deleted\n\n");
			firstMemId = toDelete.firstMemberID;
			found = 1;
		}
		else {
			fwrite(&toDelete, sizeof(org), 1, organizationDataTmp);
		}
	}
	if (!found) printf("No records with requested id: %d\n", masterID);
	fclose(handler.OrganizationDataFile);
	fclose(organizationDataTmp);
	remove(handler.dataFileName);
	rename("organization_tmp.dat", handler.dataFileName);
	deleteRecordFromIndexFile(masterID);
	deleteAllSubrecords(firstMemId);
}

void deleteRecordFromIndexFile(int id) {
	handler.dataIndexFile = fopen(handler.indexFileName, "rb");
	FILE* hospitalIndexFileTmp = fopen("hospital_tmp.ind", "wb");
	int index;
	int masterID;
	int found = 0;
	int nextIndex = -1;
	fread(&nextIndex, sizeof(int), 1, handler.dataIndexFile);
	fwrite(&nextIndex, sizeof(int), 1, hospitalIndexFileTmp);
	fseek(handler.dataIndexFile, sizeof(int), SEEK_SET);
	for (;;) {
		fread(&masterID, sizeof(int), 1, handler.dataIndexFile);
		fread(&index, sizeof(int), 1, handler.dataIndexFile);
		if (feof(handler.dataIndexFile)) break;
		if (masterID == id) {
			printf("Record in index found and deleted\n\n");
			found = 1;
		}
		else {
			fwrite(&masterID, sizeof(int), 1, hospitalIndexFileTmp);
			fwrite(&index, sizeof(int), 1, hospitalIndexFileTmp);
		}
	}
	if (!found) printf("No records in index file with requested id: %d\n", id);
	fclose(handler.dataIndexFile);
	fclose(hospitalIndexFileTmp);
	remove(handler.indexFileName);
	rename("hospital_tmp.ind", handler.indexFileName);
}

void deleteAllSubrecords(int firstDocID) {
	if (firstDocID != -1) {
		handler.MemberDataFile = fopen(handler.MemberFileName, "rb");
		member doc;
		FILE* doctorsDataTmp = fopen("doctor_tmp.dat", "wb");
		for (;;) {
			fread(&doc, sizeof(member), 1, handler.MemberDataFile);
			if (feof(handler.MemberDataFile)) break;
			if (doc.id == firstDocID) {
				printf("Delete subrecord\n");
				firstDocID = doc.nextMemberID;
			}
			else {
				fwrite(&doc, sizeof(member), 1, doctorsDataTmp);
			}
		}
		fclose(handler.MemberDataFile);
		fclose(doctorsDataTmp);
		remove(handler.MemberFileName);
		rename("doctor_tmp.dat", handler.MemberFileName);
	}

}

void clearContentOfFiles() {
	handler.OrganizationDataFile = fopen(handler.dataFileName, "wb");
	fclose(handler.OrganizationDataFile);
	handler.dataIndexFile = fopen(handler.indexFileName, "wb");
	fclose(handler.dataIndexFile);
	handler.MemberDataFile = fopen(handler.MemberFileName, "wb");
	fclose(handler.MemberDataFile);
}

void deleteMemberRecord(char command[MAX_LENGTH_OF_COMMAND], char* delims) {
	char* option = strtok(command, delims);
	int masterID = atoi(strtok(NULL, delims));
	int slaveID = atoi(strtok(NULL, delims));
	int masterIndex = searchForOrganizationIndex(masterID);
	org hosp = organizationWithIndex(masterIndex);
	removeSubrecord(&hosp, slaveID);
}

void removeSubrecord(org* hosp, int slaveID) {
	int recordID = hosp->firstMemberID;
	if (recordID != -1) {
		member doc;
		member nextDoctor;
		member previousDoctor;
		handler.MemberDataFile = fopen(handler.MemberFileName, "rb");
		FILE* doctorDataTemp = fopen("doctor_temp.dat", "wb+");
		int found = 0;
		for (;;) {
			fread(&doc, sizeof(member), 1, handler.MemberDataFile);

			if (feof(handler.MemberDataFile)) break;
			if (doc.nextMemberID == -1) {
				printf("Record with id: %d deleted\n\n", slaveID);
				found = 1;
				member previous;
				fclose(doctorDataTemp);
				doctorDataTemp = fopen("doctor_temp.dat", "rb+");
				fseek(doctorDataTemp, -1 * (int)sizeof(member), SEEK_END);
				fread(&previous, sizeof(member), 1, doctorDataTemp);
				previous.nextMemberID = -1;
				fseek(doctorDataTemp, -1 * (int)sizeof(member), SEEK_END);
				fwrite(&previous, sizeof(member), 1, doctorDataTemp);
				break;
			}
			if (doc.id == slaveID) {
				fread(&nextDoctor, sizeof(member), 1, handler.MemberDataFile);
				fclose(doctorDataTemp);
				doctorDataTemp = fopen("doctor_temp.dat", "rb+");
				if (doesFileHasContent(doctorDataTemp)) {
					fseek(doctorDataTemp, -1 * (int)sizeof(member), SEEK_END);
					fread(&previousDoctor, sizeof(member), 1, doctorDataTemp);
					previousDoctor.nextMemberID = nextDoctor.id;
					fseek(handler.MemberDataFile, -1 * (int)sizeof(member), SEEK_CUR);
					fseek(doctorDataTemp, -1 * (int)sizeof(member), SEEK_END);
					fwrite(&previousDoctor, sizeof(member), 1, doctorDataTemp);
					fseek(doctorDataTemp, 0L, SEEK_END);
				}
				else {
					fwrite(&nextDoctor, sizeof(member), 1, doctorDataTemp);
					hosp->firstMemberID = nextDoctor.id;
					int index = searchForOrganizationIndex(hosp->id);
					rewriteOrganizationRecord(*hosp, index);
					printf("Record with id: %d deleted\n\n", slaveID);
					found = 1;
				}
				printf("Record with id: %d deleted\n\n", slaveID);
				found = 1;
			}
			else {
				fwrite(&doc, sizeof(member), 1, doctorDataTemp);
			}
		}
		if (!found) printf("No record with id: %d\n\n", slaveID);
		fclose(handler.MemberDataFile);
		fclose(doctorDataTemp);
		remove(handler.MemberFileName);
		rename("doctor_temp.dat", handler.MemberFileName);
		return;
	}
	printf("This record doesn't have any subrecords\n");

}

int size_of_MemberFile()
{
	FILE* a = fopen(handler.MemberDataFile, "rb");
	fseek(a, 0, SEEK_END);
	int res = ftell(a) / sizeof(member);
	fclose(a);
	return res;
}