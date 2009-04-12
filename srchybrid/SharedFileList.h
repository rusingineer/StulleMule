//this file is part of eMule
//Copyright (C)2002-2008 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#pragma once
#include "MapKey.h"

class CKnownFileList;
class CServerConnect;
class CPartFile;
class CKnownFile;
class CPublishKeywordList;
class CSafeMemFile;
class CServer;
class CCollection;

struct UnknownFile_Struct{
	CString strName;
	CString strDirectory;
};

class CSharedFileList
{
	friend class CSharedFilesCtrl;
	friend class CClientReqSocket;

public:
	CSharedFileList(CServerConnect* in_server);
	~CSharedFileList();

	void	SendListToServer();
	void	Reload();
	bool	SafeAddKFile(CKnownFile* toadd, bool bOnlyAdd = false);
	void	RepublishFile(CKnownFile* pFile);
	void	SetOutputCtrl(CSharedFilesCtrl* in_ctrl);
	bool	RemoveFile(CKnownFile* toremove);
	CKnownFile* GetFileByID(const uchar* filehash) const;
	CKnownFile*	GetFileByIndex(int index);
	bool	IsFilePtrInList(const CKnownFile* file) const;
	void	PublishNextTurn()	{ m_lastPublishED2KFlag=true;	}
	void	CreateOfferedFilePacket(CKnownFile* cur_file, CSafeMemFile* files, CServer* pServer, CUpDownClient* pClient = NULL);
	uint64	GetDatasize(uint64 &pbytesLargest) const;
	int		GetCount()	{return m_Files_map.GetCount(); }
	int		GetHashingCount()	{return waitingforhash_list.GetCount()+currentlyhashing_list.GetCount(); }	// SLUGFILLER SafeHash
	void	UpdateFile(CKnownFile* toupdate);
  /* old code
	void	AddFilesFromDirectory(const CString& rstrDirectory);
  */
	void	AddFilesFromDirectory(const CString& rstrDirectory, bool bWithSubdir = false);	// SLUGFILLER: shareSubdir
	void	AddFileFromNewlyCreatedCollection(const CString& path, const CString& fileName);
	void	HashFailed(UnknownFile_Struct* hashed);		// SLUGFILLER: SafeHash
	void	FileHashingFinished(CKnownFile* file);
	void	ClearED2KPublishInfo();
	void	ClearKadSourcePublishInfo();
	void	Process();
	void	Publish();
	void	AddKeywords(CKnownFile* pFile);
	void	RemoveKeywords(CKnownFile* pFile);
	void	DeletePartFileInstances() const;
	bool	IsUnsharedFile(const uchar* auFileHash) const;
	void	CopySharedFileMap(CMap<CCKey,const CCKey&,CKnownFile*,CKnownFile*> &Files_Map);
	void	UpdatePartsInfo(); //MORPH - Added by SiRoB, POWERSHARE Limit
	DWORD	GetLastTimeFileMapUpdated() { return m_dwFile_map_updated; }; //MORPH - Added by SiRoB, Optimization requpfile

	CMutex	m_mutWriteList;
private:
	bool	AddFile(CKnownFile* pFile);
	void	FindSharedFiles();
	void	HashNextFile();
	// SLUGFILLER: SafeHash
	bool	IsHashing(const CString& rstrDirectory, const CString& rstrName);
	void	RemoveFromHashing(CKnownFile* hashed);
	// SLUGFILLER: SafeHash

	CMap<CCKey,const CCKey&,CKnownFile*,CKnownFile*> m_Files_map;
	DWORD m_dwFile_map_updated; //MORPH - Added by SiRoB, Optimization requpfile
	CMap<CSKey,const CSKey&, bool, bool>			 m_UnsharedFiles_map;
	CPublishKeywordList* m_keywords;
	CTypedPtrList<CPtrList, UnknownFile_Struct*> waitingforhash_list;
	CTypedPtrList<CPtrList, UnknownFile_Struct*> currentlyhashing_list;	// SLUGFILLER: SafeHash
	CServerConnect*		server;
	CSharedFilesCtrl*	output;

	uint32 m_lastPublishED2K;
	bool	 m_lastPublishED2KFlag;
	int m_currFileSrc;
	int m_currFileNotes;
	int m_currFileKey;
	uint32 m_lastPublishKadSrc;
	uint32 m_lastPublishKadNotes;

// Mighty Knife: CRC32-Tag - Public method to lock the filelist to prevent it 
// from being deleted; be careful using this not to produce deadlocks !
public:
	CMutex FileListLockMutex;
// [end] Mighty Knife

	// ==> Automatic shared files updater [MoNKi] - Stulle
	void GetSubDirs(const CString& rstrDirectory);
	void FindSubDirs();
	// <== Automatic shared files updater [MoNKi] - Stulle
};

class CAddFileThread : public CWinThread
{
	DECLARE_DYNCREATE(CAddFileThread)
protected:
	CAddFileThread();
public:
	virtual BOOL InitInstance();
	virtual int		Run();
	void	SetValues(CSharedFileList* pOwner, LPCTSTR directory, LPCTSTR filename, CPartFile* partfile = NULL);
	//MORPH START - Added by SiRoB, Import Parts [SR13]
	bool	SR13_ImportParts();
	uint16	SetPartToImport(LPCTSTR import);
	//MORPH END   - Added by SiRoB, Import Parts [SR13]
private:
	CSharedFileList* m_pOwner;
	CString			 m_strDirectory;
	CString			 m_strFilename;
	CPartFile*		 m_partfile;
	//MORPH START - Added by SiRoB, Import Parts [SR13]
	CString          m_strImport;
	CArray<uint16,uint16>	m_PartsToImport;
	//MORPH END   - Added by SiRoB, Import Parts [SR13]
};
