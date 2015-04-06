/******* Enumerations *******/
typedef enum _PP_NPAGED_LOOKASIDE_NUMBER {
    LookasideSmallIrpList = 0,
    LookasideLargeIrpList = 1,
    LookasideMdlList = 2,
    LookasideCreateInfoList = 3,
    LookasideNameBufferList = 4,
    LookasideTwilightList = 5,
    LookasideCompletionList = 6,
    LookasideMaximumList = 7

} PP_NPAGED_LOOKASIDE_NUMBER;

typedef enum _POOL_TYPE {
    NonPagedPool = 0,
    PagedPool = 1,
    NonPagedPoolMustSucceed = 2,
    DontUseThisType = 3,
    NonPagedPoolCacheAligned = 4,
    PagedPoolCacheAligned = 5,
    NonPagedPoolCacheAlignedMustS = 6,
    MaxPoolType = 7,
    NonPagedPoolSession = 32,
    PagedPoolSession = 33,
    NonPagedPoolMustSucceedSession = 34,
    DontUseThisTypeSession = 35,
    NonPagedPoolCacheAlignedSession = 36,
    PagedPoolCacheAlignedSession = 37,
    NonPagedPoolCacheAlignedMustSSession = 38

} POOL_TYPE;

typedef enum _MEMORY_CACHING_TYPE {
    MmNonCached = 0,
    MmCached = 1,
    MmWriteCombined = 2,
    MmHardwareCoherentCached = 3,
    MmNonCachedUnordered = 4,
    MmUSWCCached = 5,
    MmMaximumCacheType = 6

} MEMORY_CACHING_TYPE;

typedef enum _MI_PFN_CACHE_ATTRIBUTE {
    MiNonCached = 0,
    MiCached = 1,
    MiWriteCombined = 2,
    MiNotMapped = 3

} MI_PFN_CACHE_ATTRIBUTE;

typedef enum _PF_SCENARIO_TYPE {
    PfApplicationLaunchScenarioType = 0,
    PfSystemBootScenarioType = 1,
    PfMaxScenarioType = 2

} PF_SCENARIO_TYPE;

typedef enum _SYSTEM_POWER_STATE {
    PowerSystemUnspecified = 0,
    PowerSystemWorking = 1,
    PowerSystemSleeping1 = 2,
    PowerSystemSleeping2 = 3,
    PowerSystemSleeping3 = 4,
    PowerSystemHibernate = 5,
    PowerSystemShutdown = 6,
    PowerSystemMaximum = 7

} SYSTEM_POWER_STATE;

typedef enum _DEVICE_POWER_STATE {
    PowerDeviceUnspecified = 0,
    PowerDeviceD0 = 1,
    PowerDeviceD1 = 2,
    PowerDeviceD2 = 3,
    PowerDeviceD3 = 4,
    PowerDeviceMaximum = 5

} DEVICE_POWER_STATE;

typedef enum _MMLISTS {
    ZeroedPageList = 0,
    FreePageList = 1,
    StandbyPageList = 2,
    ModifiedPageList = 3,
    ModifiedNoWritePageList = 4,
    BadPageList = 5,
    ActiveAndValid = 6,
    TransitionPage = 7

} MMLISTS;

typedef enum _SECURITY_IMPERSONATION_LEVEL {
    SecurityAnonymous = 0,
    SecurityIdentification = 1,
    SecurityImpersonation = 2,
    SecurityDelegation = 3

} SECURITY_IMPERSONATION_LEVEL;

typedef enum _PNP_DEVNODE_STATE {
    DeviceNodeUnspecified = 768,
    DeviceNodeUninitialized = 769,
    DeviceNodeInitialized = 770,
    DeviceNodeDriversAdded = 771,
    DeviceNodeResourcesAssigned = 772,
    DeviceNodeStartPending = 773,
    DeviceNodeStartCompletion = 774,
    DeviceNodeStartPostWork = 775,
    DeviceNodeStarted = 776,
    DeviceNodeQueryStopped = 777,
    DeviceNodeStopped = 778,
    DeviceNodeRestartCompletion = 779,
    DeviceNodeEnumeratePending = 780,
    DeviceNodeEnumerateCompletion = 781,
    DeviceNodeAwaitingQueuedDeletion = 782,
    DeviceNodeAwaitingQueuedRemoval = 783,
    DeviceNodeQueryRemoved = 784,
    DeviceNodeRemovePendingCloses = 785,
    DeviceNodeRemoved = 786,
    DeviceNodeDeletePendingCloses = 787,
    DeviceNodeDeleted = 788

} PNP_DEVNODE_STATE;

typedef enum _INTERFACE_TYPE {
    InterfaceTypeUndefined = -1,
    Internal = 0,
    Isa = 1,
    Eisa = 2,
    MicroChannel = 3,
    TurboChannel = 4,
    xxxPCIBus = 5,
    VMEBus = 6,
    NuBus = 7,
    PCMCIABus = 8,
    CBus = 9,
    MPIBus = 10,
    MPSABus = 11,
    ProcessorInternal = 12,
    InternalPowerBus = 13,
    PNPISABus = 14,
    PNPBus = 15,
    MaximumInterfaceType = 16

} INTERFACE_TYPE;

typedef enum _PROFILE_STATUS {
    DOCK_NOTDOCKDEVICE = 0,
    DOCK_QUIESCENT = 1,
    DOCK_ARRIVING = 2,
    DOCK_DEPARTING = 3,
    DOCK_EJECTIRP_COMPLETED = 4

} PROFILE_STATUS;

typedef enum _LSA_FOREST_TRUST_RECORD_TYPE {
    ForestTrustTopLevelName = 0,
    ForestTrustTopLevelNameEx = 1,
    ForestTrustDomainInfo = 2,
    ForestTrustRecordTypeLast = 2

} LSA_FOREST_TRUST_RECORD_TYPE;

typedef enum _POWER_ACTION {
    PowerActionNone = 0,
    PowerActionReserved = 1,
    PowerActionSleep = 2,
    PowerActionHibernate = 3,
    PowerActionShutdown = 4,
    PowerActionShutdownReset = 5,
    PowerActionShutdownOff = 6,
    PowerActionWarmEject = 7

} POWER_ACTION;

typedef enum _KWAIT_REASON {
    Executive = 0,
    FreePage = 1,
    PageIn = 2,
    PoolAllocation = 3,
    DelayExecution = 4,
    Suspended = 5,
    UserRequest = 6,
    WrExecutive = 7,
    WrFreePage = 8,
    WrPageIn = 9,
    WrPoolAllocation = 10,
    WrDelayExecution = 11,
    WrSuspended = 12,
    WrUserRequest = 13,
    WrEventPair = 14,
    WrQueue = 15,
    WrLpcReceive = 16,
    WrLpcReply = 17,
    WrVirtualMemory = 18,
    WrPageOut = 19,
    WrRendezvous = 20,
    Spare2 = 21,
    Spare3 = 22,
    Spare4 = 23,
    Spare5 = 24,
    Spare6 = 25,
    WrKernel = 26,
    MaximumWaitReason = 27

} KWAIT_REASON;

typedef enum _NT_PRODUCT_TYPE {
    NtProductWinNt = 1,
    NtProductLanManNt = 2,
    NtProductServer = 3

} NT_PRODUCT_TYPE;

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE {
    StandardDesign = 0,
    NEC98x86 = 1,
    EndAlternatives = 2

} ALTERNATIVE_ARCHITECTURE_TYPE;

typedef enum _OB_OPEN_REASON {
    ObCreateHandle = 0,
    ObOpenHandle = 1,
    ObDuplicateHandle = 2,
    ObInheritHandle = 3,
    ObMaxOpenReason = 4

} OB_OPEN_REASON;

typedef enum _SECURITY_OPERATION_CODE {
    SetSecurityDescriptor = 0,
    QuerySecurityDescriptor = 1,
    DeleteSecurityDescriptor = 2,
    AssignSecurityDescriptor = 3

} SECURITY_OPERATION_CODE;

typedef enum _MEMORY_CACHING_TYPE_ORIG {
    MmFrameBufferCached = 2

} MEMORY_CACHING_TYPE_ORIG;

typedef enum _EXCEPTION_DISPOSITION {
    ExceptionContinueExecution = 0,
    ExceptionContinueSearch = 1,
    ExceptionNestedException = 2,
    ExceptionCollidedUnwind = 3

} EXCEPTION_DISPOSITION;

typedef enum _PCI_SIGNATURE {
    PciPdoExtensionType = 1768116272,
    PciFdoExtensionType = 1768116273,
    PciArb_Io = 1768116274,
    PciArb_Memory = 1768116275,
    PciArb_Interrupt = 1768116276,
    PciArb_BusNumber = 1768116277,
    PciTrans_Interrupt = 1768116278,
    PciInterface_BusHandler = 1768116279,
    PciInterface_IntRouteHandler = 1768116280,
    PciInterface_PciCb = 1768116281,
    PciInterface_LegacyDeviceDetection = 1768116282,
    PciInterface_PmeHandler = 1768116283,
    PciInterface_DevicePresent = 1768116284,
    PciInterface_NativeIde = 1768116285,
    PciInterface_AgpTarget = 1768116286

} PCI_SIGNATURE;

typedef enum _MODE {
    KernelMode = 0,
    UserMode = 1,
    MaximumMode = 2

} MODE;

typedef enum _IO_ALLOCATION_ACTION {
    KeepObject = 1,
    DeallocateObject = 2,
    DeallocateObjectKeepRegisters = 3

} IO_ALLOCATION_ACTION;

typedef enum _POP_POLICY_DEVICE_TYPE {
    PolicyDeviceSystemButton = 0,
    PolicyDeviceThermalZone = 1,
    PolicyDeviceBattery = 2,
    PolicyInitiatePowerActionAPI = 3,
    PolicySetPowerStateAPI = 4,
    PolicyImmediateDozeS4 = 5,
    PolicySystemIdle = 6

} POP_POLICY_DEVICE_TYPE;

typedef enum _POLICY_AUDIT_EVENT_TYPE {
    AuditCategorySystem = 0,
    AuditCategoryLogon = 1,
    AuditCategoryObjectAccess = 2,
    AuditCategoryPrivilegeUse = 3,
    AuditCategoryDetailedTracking = 4,
    AuditCategoryPolicyChange = 5,
    AuditCategoryAccountManagement = 6,
    AuditCategoryDirectoryServiceAccess = 7,
    AuditCategoryAccountLogon = 8

} POLICY_AUDIT_EVENT_TYPE;

typedef enum _VI_DEADLOCK_RESOURCE_TYPE {
    VfDeadlockUnknown = 0,
    VfDeadlockMutex = 1,
    VfDeadlockFastMutex = 2,
    VfDeadlockFastMutexUnsafe = 3,
    VfDeadlockSpinLock = 4,
    VfDeadlockQueuedSpinLock = 5,
    VfDeadlockTypeMaximum = 6

} VI_DEADLOCK_RESOURCE_TYPE;

typedef enum _KINTERRUPT_MODE {
    LevelSensitive = 0,
    Latched = 1

} KINTERRUPT_MODE;

typedef enum _PCI_DISPATCH_STYLE {
    IRP_COMPLETE = 0,
    IRP_DOWNWARD = 1,
    IRP_UPWARD = 2,
    IRP_DISPATCH = 3

} PCI_DISPATCH_STYLE;

typedef enum _ReplacesCorHdrNumericDefines {
    COMIMAGE_FLAGS_ILONLY = 1,
    COMIMAGE_FLAGS_32BITREQUIRED = 2,
    COMIMAGE_FLAGS_IL_LIBRARY = 4,
    COMIMAGE_FLAGS_STRONGNAMESIGNED = 8,
    COMIMAGE_FLAGS_TRACKDEBUGDATA = 65536,
    COR_VERSION_MAJOR_V2 = 2,
    COR_VERSION_MAJOR = 2,
    COR_VERSION_MINOR = 0,
    COR_DELETED_NAME_LENGTH = 8,
    COR_VTABLEGAP_NAME_LENGTH = 8,
    NATIVE_TYPE_MAX_CB = 1,
    COR_ILMETHOD_SECT_SMALL_MAX_DATASIZE = 255,
    IMAGE_COR_MIH_METHODRVA = 1,
    IMAGE_COR_MIH_EHRVA = 2,
    IMAGE_COR_MIH_BASICBLOCK = 8,
    COR_VTABLE_32BIT = 1,
    COR_VTABLE_64BIT = 2,
    COR_VTABLE_FROM_UNMANAGED = 4,
    COR_VTABLE_CALL_MOST_DERIVED = 16,
    IMAGE_COR_EATJ_THUNK_SIZE = 32,
    MAX_CLASS_NAME = 1024,
    MAX_PACKAGE_NAME = 1024

} ReplacesCorHdrNumericDefines;

typedef enum _MEMORY_TYPE {
    MemoryExceptionBlock = 0,
    MemorySystemBlock = 1,
    MemoryFree = 2,
    MemoryBad = 3,
    MemoryLoadedProgram = 4,
    MemoryFirmwareTemporary = 5,
    MemoryFirmwarePermanent = 6,
    MemoryFreeContiguous = 7,
    MemorySpecialMemory = 8,
    MemoryMaximum = 9

} MEMORY_TYPE;

typedef enum _ARBITER_ACTION {
    ArbiterActionTestAllocation = 0,
    ArbiterActionRetestAllocation = 1,
    ArbiterActionCommitAllocation = 2,
    ArbiterActionRollbackAllocation = 3,
    ArbiterActionQueryAllocatedResources = 4,
    ArbiterActionWriteReservedResources = 5,
    ArbiterActionQueryConflict = 6,
    ArbiterActionQueryArbitrate = 7,
    ArbiterActionAddReserved = 8,
    ArbiterActionBootAllocation = 9

} ARBITER_ACTION;

typedef enum _TOKEN_TYPE {
    TokenPrimary = 1,
    TokenImpersonation = 2

} TOKEN_TYPE;

typedef enum _PS_QUOTA_TYPE {
    PsNonPagedPool = 0,
    PsPagedPool = 1,
    PsPageFile = 2,
    PsQuotaTypes = 3

} PS_QUOTA_TYPE;

typedef enum _PNP_VETO_TYPE {
    PNP_VetoTypeUnknown = 0,
    PNP_VetoLegacyDevice = 1,
    PNP_VetoPendingClose = 2,
    PNP_VetoWindowsApp = 3,
    PNP_VetoWindowsService = 4,
    PNP_VetoOutstandingOpen = 5,
    PNP_VetoDevice = 6,
    PNP_VetoDriver = 7,
    PNP_VetoIllegalDeviceRequest = 8,
    PNP_VetoInsufficientPower = 9,
    PNP_VetoNonDisableable = 10,
    PNP_VetoLegacyDriver = 11,
    PNP_VetoInsufficientRights = 12

} PNP_VETO_TYPE;

typedef enum _BUS_DATA_TYPE {
    ConfigurationSpaceUndefined = -1,
    Cmos = 0,
    EisaConfiguration = 1,
    Pos = 2,
    CbusConfiguration = 3,
    PCIConfiguration = 4,
    VMEConfiguration = 5,
    NuBusConfiguration = 6,
    PCMCIAConfiguration = 7,
    MPIConfiguration = 8,
    MPSAConfiguration = 9,
    PNPISAConfiguration = 10,
    SgiInternalConfiguration = 11,
    MaximumBusDataType = 12

} BUS_DATA_TYPE;

typedef enum _MMSYSTEM_PTE_POOL_TYPE {
    SystemPteSpace = 0,
    NonPagedPoolExpansion = 1,
    MaximumPtePoolTypes = 2

} MMSYSTEM_PTE_POOL_TYPE;

typedef enum _PROXY_CLASS {
    ProxyFull = 0,
    ProxyService = 1,
    ProxyTree = 2,
    ProxyDirectory = 3

} PROXY_CLASS;

typedef enum _FILE_INFORMATION_CLASS {
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation = 2,
    FileBothDirectoryInformation = 3,
    FileBasicInformation = 4,
    FileStandardInformation = 5,
    FileInternalInformation = 6,
    FileEaInformation = 7,
    FileAccessInformation = 8,
    FileNameInformation = 9,
    FileRenameInformation = 10,
    FileLinkInformation = 11,
    FileNamesInformation = 12,
    FileDispositionInformation = 13,
    FilePositionInformation = 14,
    FileFullEaInformation = 15,
    FileModeInformation = 16,
    FileAlignmentInformation = 17,
    FileAllInformation = 18,
    FileAllocationInformation = 19,
    FileEndOfFileInformation = 20,
    FileAlternateNameInformation = 21,
    FileStreamInformation = 22,
    FilePipeInformation = 23,
    FilePipeLocalInformation = 24,
    FilePipeRemoteInformation = 25,
    FileMailslotQueryInformation = 26,
    FileMailslotSetInformation = 27,
    FileCompressionInformation = 28,
    FileObjectIdInformation = 29,
    FileCompletionInformation = 30,
    FileMoveClusterInformation = 31,
    FileQuotaInformation = 32,
    FileReparsePointInformation = 33,
    FileNetworkOpenInformation = 34,
    FileAttributeTagInformation = 35,
    FileTrackingInformation = 36,
    FileIdBothDirectoryInformation = 37,
    FileIdFullDirectoryInformation = 38,
    FileValidDataLengthInformation = 39,
    FileShortNameInformation = 40,
    FileMaximumInformation = 41

} FILE_INFORMATION_CLASS;

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation = 1,
    FileFsLabelInformation = 2,
    FileFsSizeInformation = 3,
    FileFsDeviceInformation = 4,
    FileFsAttributeInformation = 5,
    FileFsControlInformation = 6,
    FileFsFullSizeInformation = 7,
    FileFsObjectIdInformation = 8,
    FileFsDriverPathInformation = 9,
    FileFsMaximumInformation = 10

} FSINFOCLASS;

typedef enum _DEVICE_RELATION_TYPE {
    BusRelations = 0,
    EjectionRelations = 1,
    PowerRelations = 2,
    RemovalRelations = 3,
    TargetDeviceRelation = 4,
    SingleBusRelations = 5

} DEVICE_RELATION_TYPE;

typedef enum _BUS_QUERY_ID_TYPE {
    BusQueryDeviceID = 0,
    BusQueryHardwareIDs = 1,
    BusQueryCompatibleIDs = 2,
    BusQueryInstanceID = 3,
    BusQueryDeviceSerialNumber = 4

} BUS_QUERY_ID_TYPE;

typedef enum _DEVICE_TEXT_TYPE {
    DeviceTextDescription = 0,
    DeviceTextLocationInformation = 1

} DEVICE_TEXT_TYPE;

typedef enum _DEVICE_USAGE_NOTIFICATION_TYPE {
    DeviceUsageTypeUndefined = 0,
    DeviceUsageTypePaging = 1,
    DeviceUsageTypeHibernation = 2,
    DeviceUsageTypeDumpFile = 3

} DEVICE_USAGE_NOTIFICATION_TYPE;

typedef enum _POWER_STATE_TYPE {
    SystemPowerState = 0,
    DevicePowerState = 1

} POWER_STATE_TYPE;

typedef enum _PLUGPLAY_EVENT_CATEGORY {
    HardwareProfileChangeEvent = 0,
    TargetDeviceChangeEvent = 1,
    DeviceClassChangeEvent = 2,
    CustomDeviceEvent = 3,
    DeviceInstallEvent = 4,
    DeviceArrivalEvent = 5,
    PowerEvent = 6,
    VetoEvent = 7,
    BlockedDriverEvent = 8,
    MaxPlugEventCategory = 9

} PLUGPLAY_EVENT_CATEGORY;

typedef enum _ARBITER_REQUEST_SOURCE {
    ArbiterRequestUndefined = -1,
    ArbiterRequestLegacyReported = 0,
    ArbiterRequestHalReported = 1,
    ArbiterRequestLegacyAssigned = 2,
    ArbiterRequestPnpDetected = 3,
    ArbiterRequestPnpEnumerated = 4

} ARBITER_REQUEST_SOURCE;

typedef enum _ARBITER_RESULT {
    ArbiterResultUndefined = -1,
    ArbiterResultSuccess = 0,
    ArbiterResultExternalConflict = 1,
    ArbiterResultNullRequest = 2

} ARBITER_RESULT;

typedef enum _FS_FILTER_SECTION_SYNC_TYPE {
    SyncTypeOther = 0,
    SyncTypeCreateSection = 1

} FS_FILTER_SECTION_SYNC_TYPE;

/*******  Structures  *******/
typedef struct _SECURITY_SUBJECT_CONTEXT { // 0x10 bytes
    uint32_t ClientToken ; // offset 0x0
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel ; // offset 0x4
    uint32_t PrimaryToken ; // offset 0x8
    uint32_t ProcessAuditId ; // offset 0xc

} SECURITY_SUBJECT_CONTEXT, *PSECURITY_SUBJECT_CONTEXT, **PPSECURITY_SUBJECT_CONTEXT ;

typedef struct _LUID { // 0x8 bytes
    uint32_t LowPart ; // offset 0x0
    int32_t HighPart ; // offset 0x4

} LUID, *PLUID, **PPLUID ;

typedef struct _LUID_AND_ATTRIBUTES { // 0xc bytes
    LUID Luid ; // offset 0x0
    uint32_t Attributes ; // offset 0x8

} LUID_AND_ATTRIBUTES, *PLUID_AND_ATTRIBUTES, **PPLUID_AND_ATTRIBUTES ;

typedef struct _INITIAL_PRIVILEGE_SET { // 0x2c bytes
    uint32_t PrivilegeCount ; // offset 0x0
    uint32_t Control ; // offset 0x4
    LUID_AND_ATTRIBUTES Privilege[3] ; // offset 0x8

} INITIAL_PRIVILEGE_SET, *PINITIAL_PRIVILEGE_SET, **PPINITIAL_PRIVILEGE_SET ;

typedef struct _UNICODE_STRING { // 0x8 bytes
    uint16_t Length ; // offset 0x0
    uint16_t MaximumLength ; // offset 0x2
    uint32_t Buffer ; // offset 0x4

} UNICODE_STRING, *PUNICODE_STRING, **PPUNICODE_STRING ;

typedef struct _PRIVILEGE_SET { // 0x14 bytes
    uint32_t PrivilegeCount ; // offset 0x0
    uint32_t Control ; // offset 0x4
    LUID_AND_ATTRIBUTES Privilege[1] ; // offset 0x8

} PRIVILEGE_SET, *PPRIVILEGE_SET, **PPPRIVILEGE_SET ;

typedef struct _ACCESS_STATE { // 0x74 bytes
    LUID OperationID ; // offset 0x0
    uint8_t SecurityEvaluated ; // offset 0x8
    uint8_t GenerateAudit ; // offset 0x9
    uint8_t GenerateOnClose ; // offset 0xa
    uint8_t PrivilegesAllocated ; // offset 0xb
    uint32_t Flags ; // offset 0xc
    uint32_t RemainingDesiredAccess ; // offset 0x10
    uint32_t PreviouslyGrantedAccess ; // offset 0x14
    uint32_t OriginalDesiredAccess ; // offset 0x18
    SECURITY_SUBJECT_CONTEXT SubjectSecurityContext ; // offset 0x1c
    uint32_t SecurityDescriptor ; // offset 0x2c
    uint32_t AuxData ; // offset 0x30
    union {
        union {
        INITIAL_PRIVILEGE_SET InitialPrivilegeSet ; // offset 0x0
        PRIVILEGE_SET PrivilegeSet ; // offset 0x0
        };
    } Privileges ; // offset 0x34
    uint8_t AuditPrivileges ; // offset 0x60
    UNICODE_STRING ObjectName ; // offset 0x64
    UNICODE_STRING ObjectTypeName ; // offset 0x6c

} ACCESS_STATE, *PACCESS_STATE, **PPACCESS_STATE ;

typedef struct _PCI_MN_DISPATCH_TABLE { // 0x8 bytes
    PCI_DISPATCH_STYLE DispatchStyle ; // offset 0x0
    uint32_t DispatchFunction ; // offset 0x4

} PCI_MN_DISPATCH_TABLE, *PPCI_MN_DISPATCH_TABLE, **PPPCI_MN_DISPATCH_TABLE ;

typedef struct _LIST_ENTRY { // 0x8 bytes
    uint32_t Flink ; // offset 0x0
    uint32_t Blink ; // offset 0x4

} LIST_ENTRY, *PLIST_ENTRY, **PPLIST_ENTRY ;

typedef struct _HEAP_FREE_ENTRY { // 0x10 bytes
    uint16_t Size ; // offset 0x0
    uint16_t PreviousSize ; // offset 0x2
    uint32_t SubSegmentCode ; // offset 0x0
    uint8_t SmallTagIndex ; // offset 0x4
    uint8_t Flags ; // offset 0x5
    uint8_t UnusedBytes ; // offset 0x6
    volatile uint8_t SegmentIndex ; // offset 0x7
    LIST_ENTRY FreeList ; // offset 0x8

} HEAP_FREE_ENTRY, *PHEAP_FREE_ENTRY, **PPHEAP_FREE_ENTRY ;

typedef struct _DEVICE_MAP { // 0x30 bytes
    uint32_t DosDevicesDirectory ; // offset 0x0
    uint32_t GlobalDosDevicesDirectory ; // offset 0x4
    uint32_t ReferenceCount ; // offset 0x8
    uint32_t DriveMap ; // offset 0xc
    uint8_t DriveType[32] ; // offset 0x10

} DEVICE_MAP, *PDEVICE_MAP, **PPDEVICE_MAP ;

typedef struct _OBJECT_DIRECTORY_ENTRY { // 0x8 bytes
    uint32_t ChainLink ; // offset 0x0
    uint32_t Object ; // offset 0x4

} OBJECT_DIRECTORY_ENTRY, *POBJECT_DIRECTORY_ENTRY, **PPOBJECT_DIRECTORY_ENTRY ;

typedef struct _EX_PUSH_LOCK { // 0x4 bytes
    union {
    uint32_t Waiting : 1 ; // offset 0x0
    uint32_t Exclusive : 1 ; // offset 0x0
    uint32_t Shared : 30 ; // offset 0x0
    uint32_t Value ; // offset 0x0
    uint32_t Ptr ; // offset 0x0
    };

} EX_PUSH_LOCK, *PEX_PUSH_LOCK, **PPEX_PUSH_LOCK ;

typedef struct _OBJECT_DIRECTORY { // 0xa4 bytes
    uint32_t HashBuckets[37] ; // offset 0x0
    EX_PUSH_LOCK Lock ; // offset 0x94
    uint32_t DeviceMap ; // offset 0x98
    uint32_t SessionId ; // offset 0x9c
    uint16_t Reserved ; // offset 0xa0
    uint16_t SymbolicLinkUsageCount ; // offset 0xa2

} OBJECT_DIRECTORY, *POBJECT_DIRECTORY, **PPOBJECT_DIRECTORY ;

typedef struct _IO_SECURITY_CONTEXT { // 0x10 bytes
    uint32_t SecurityQos ; // offset 0x0
    uint32_t AccessState ; // offset 0x4
    uint32_t DesiredAccess ; // offset 0x8
    uint32_t FullCreateOptions ; // offset 0xc

} IO_SECURITY_CONTEXT, *PIO_SECURITY_CONTEXT, **PPIO_SECURITY_CONTEXT ;

typedef struct _MMFREE_POOL_ENTRY { // 0x14 bytes
    LIST_ENTRY List ; // offset 0x0
    uint32_t Size ; // offset 0x8
    uint32_t Signature ; // offset 0xc
    uint32_t Owner ; // offset 0x10

} MMFREE_POOL_ENTRY, *PMMFREE_POOL_ENTRY, **PPMMFREE_POOL_ENTRY ;

typedef struct _MMWSLENTRY { // 0x4 bytes
    union {
    uint32_t Valid : 1 ; // offset 0x0
    uint32_t LockedInWs : 1 ; // offset 0x0
    uint32_t LockedInMemory : 1 ; // offset 0x0
    uint32_t Protection : 5 ; // offset 0x0
    uint32_t Hashed : 1 ; // offset 0x0
    uint32_t Direct : 1 ; // offset 0x0
    uint32_t Age : 2 ; // offset 0x0
    uint32_t VirtualPageNumber : 20 ; // offset 0x0
    };

} MMWSLENTRY, *PMMWSLENTRY, **PPMMWSLENTRY ;

typedef struct _MMWSLE { // 0x4 bytes
    union {
        union {
        uint32_t VirtualAddress ; // offset 0x0
        uint32_t Long ; // offset 0x0
        MMWSLENTRY e1 ; // offset 0x0
        };
    } u1 ; // offset 0x0

} MMWSLE, *PMMWSLE, **PPMMWSLE ;

typedef union _FS_FILTER_PARAMETERS { // 0x14 bytes
    union {
    struct {
            uint32_t EndingOffset ; // offset 0x0
            uint32_t ResourceToRelease ; // offset 0x4
        } AcquireForModifiedPageWriter ; // offset 0x0
    struct {
            uint32_t ResourceToRelease ; // offset 0x0
        } ReleaseForModifiedPageWriter ; // offset 0x0
    struct {
            FS_FILTER_SECTION_SYNC_TYPE SyncType ; // offset 0x0
            uint32_t PageProtection ; // offset 0x4
        } AcquireForSectionSynchronization ; // offset 0x0
    struct {
            uint32_t Argument1 ; // offset 0x0
            uint32_t Argument2 ; // offset 0x4
            uint32_t Argument3 ; // offset 0x8
            uint32_t Argument4 ; // offset 0xc
            uint32_t Argument5 ; // offset 0x10
        } Others ; // offset 0x0
    };

} FS_FILTER_PARAMETERS, *PFS_FILTER_PARAMETERS, **PPFS_FILTER_PARAMETERS ;

typedef struct _FS_FILTER_CALLBACK_DATA { // 0x24 bytes
    uint32_t SizeOfFsFilterCallbackData ; // offset 0x0
    uint8_t Operation ; // offset 0x4
    uint8_t Reserved ; // offset 0x5
    uint32_t DeviceObject ; // offset 0x8
    uint32_t FileObject ; // offset 0xc
    FS_FILTER_PARAMETERS Parameters ; // offset 0x10

} FS_FILTER_CALLBACK_DATA, *PFS_FILTER_CALLBACK_DATA, **PPFS_FILTER_CALLBACK_DATA ;

typedef struct _RTL_CRITICAL_SECTION { // 0x18 bytes
    uint32_t DebugInfo ; // offset 0x0
    int32_t LockCount ; // offset 0x4
    int32_t RecursionCount ; // offset 0x8
    uint32_t OwningThread ; // offset 0xc
    uint32_t LockSemaphore ; // offset 0x10
    uint32_t SpinCount ; // offset 0x14

} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION, **PPRTL_CRITICAL_SECTION ;

typedef struct _RTL_HANDLE_TABLE { // 0x20 bytes
    uint32_t MaximumNumberOfHandles ; // offset 0x0
    uint32_t SizeOfHandleTableEntry ; // offset 0x4
    uint32_t Reserved[2] ; // offset 0x8
    uint32_t FreeHandles ; // offset 0x10
    uint32_t CommittedHandles ; // offset 0x14
    uint32_t UnCommittedHandles ; // offset 0x18
    uint32_t MaxReservedHandles ; // offset 0x1c

} RTL_HANDLE_TABLE, *PRTL_HANDLE_TABLE, **PPRTL_HANDLE_TABLE ;

typedef struct _RTL_ATOM_TABLE_ENTRY { // 0x10 bytes
    uint32_t HashLink ; // offset 0x0
    uint16_t HandleIndex ; // offset 0x4
    uint16_t Atom ; // offset 0x6
    uint16_t ReferenceCount ; // offset 0x8
    uint8_t Flags ; // offset 0xa
    uint8_t NameLength ; // offset 0xb
    uint16_t Name[1] ; // offset 0xc

} RTL_ATOM_TABLE_ENTRY, *PRTL_ATOM_TABLE_ENTRY, **PPRTL_ATOM_TABLE_ENTRY ;

typedef struct _RTL_ATOM_TABLE { // 0x44 bytes
    uint32_t Signature ; // offset 0x0
    RTL_CRITICAL_SECTION CriticalSection ; // offset 0x4
    RTL_HANDLE_TABLE RtlHandleTable ; // offset 0x1c
    uint32_t NumberOfBuckets ; // offset 0x3c
    uint32_t Buckets[1] ; // offset 0x40

} RTL_ATOM_TABLE, *PRTL_ATOM_TABLE, **PPRTL_ATOM_TABLE ;

typedef struct _DBGKD_LOAD_SYMBOLS32 { // 0x18 bytes
    uint32_t PathNameLength ; // offset 0x0
    uint32_t BaseOfDll ; // offset 0x4
    uint32_t ProcessId ; // offset 0x8
    uint32_t CheckSum ; // offset 0xc
    uint32_t SizeOfImage ; // offset 0x10
    uint8_t UnloadSymbols ; // offset 0x14

} DBGKD_LOAD_SYMBOLS32, *PDBGKD_LOAD_SYMBOLS32, **PPDBGKD_LOAD_SYMBOLS32 ;

typedef union _LARGE_INTEGER { // 0x8 bytes
    uint32_t LowPart ; // offset 0x0
    int32_t HighPart ; // offset 0x4
    union {
    struct {
            uint32_t LowPart ; // offset 0x0
            int32_t HighPart ; // offset 0x4
        } u ; // offset 0x0
    int64_t QuadPart ; // offset 0x0
    };

} LARGE_INTEGER, *PLARGE_INTEGER, **PPLARGE_INTEGER ;

typedef struct _FILE_BASIC_INFORMATION { // 0x28 bytes
    LARGE_INTEGER CreationTime ; // offset 0x0
    LARGE_INTEGER LastAccessTime ; // offset 0x8
    LARGE_INTEGER LastWriteTime ; // offset 0x10
    LARGE_INTEGER ChangeTime ; // offset 0x18
    uint32_t FileAttributes ; // offset 0x20

} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION, **PPFILE_BASIC_INFORMATION ;

typedef struct _PCI_BUS_INTERFACE_STANDARD { // 0x20 bytes
    uint16_t Size ; // offset 0x0
    uint16_t Version ; // offset 0x2
    uint32_t Context ; // offset 0x4
    uint32_t InterfaceReference ; // offset 0x8
    uint32_t InterfaceDereference ; // offset 0xc
    uint32_t ReadConfig ; // offset 0x10
    uint32_t WriteConfig ; // offset 0x14
    uint32_t PinToLine ; // offset 0x18
    uint32_t LineToPin ; // offset 0x1c

} PCI_BUS_INTERFACE_STANDARD, *PPCI_BUS_INTERFACE_STANDARD, **PPPCI_BUS_INTERFACE_STANDARD ;

typedef struct _OBJECT_HEADER_CREATOR_INFO { // 0x10 bytes
    LIST_ENTRY TypeList ; // offset 0x0
    uint32_t CreatorUniqueProcess ; // offset 0x8
    uint16_t CreatorBackTraceIndex ; // offset 0xc
    uint16_t Reserved ; // offset 0xe

} OBJECT_HEADER_CREATOR_INFO, *POBJECT_HEADER_CREATOR_INFO, **PPOBJECT_HEADER_CREATOR_INFO ;

typedef struct _DISPATCHER_HEADER { // 0x10 bytes
    uint8_t Type ; // offset 0x0
    uint8_t Absolute ; // offset 0x1
    uint8_t Size ; // offset 0x2
    uint8_t Inserted ; // offset 0x3
    int32_t SignalState ; // offset 0x4
    LIST_ENTRY WaitListHead ; // offset 0x8

} DISPATCHER_HEADER, *PDISPATCHER_HEADER, **PPDISPATCHER_HEADER ;

typedef union _ULARGE_INTEGER { // 0x8 bytes
    uint32_t LowPart ; // offset 0x0
    uint32_t HighPart ; // offset 0x4
    union {
    struct {
            uint32_t LowPart ; // offset 0x0
            uint32_t HighPart ; // offset 0x4
        } u ; // offset 0x0
    uint64_t QuadPart ; // offset 0x0
    };

} ULARGE_INTEGER, *PULARGE_INTEGER, **PPULARGE_INTEGER ;

typedef struct _KTIMER { // 0x28 bytes
    DISPATCHER_HEADER Header ; // offset 0x0
    ULARGE_INTEGER DueTime ; // offset 0x10
    LIST_ENTRY TimerListEntry ; // offset 0x18
    uint32_t Dpc ; // offset 0x20
    int32_t Period ; // offset 0x24

} KTIMER, *PKTIMER, **PPKTIMER ;

typedef struct _KAPC { // 0x30 bytes
    int16_t Type ; // offset 0x0
    int16_t Size ; // offset 0x2
    uint32_t Spare0 ; // offset 0x4
    uint32_t Thread ; // offset 0x8
    LIST_ENTRY ApcListEntry ; // offset 0xc
    uint32_t KernelRoutine ; // offset 0x14
    uint32_t RundownRoutine ; // offset 0x18
    uint32_t NormalRoutine ; // offset 0x1c
    uint32_t NormalContext ; // offset 0x20
    uint32_t SystemArgument1 ; // offset 0x24
    uint32_t SystemArgument2 ; // offset 0x28
    uint8_t ApcStateIndex ; // offset 0x2c
    uint8_t ApcMode ; // offset 0x2d
    uint8_t Inserted ; // offset 0x2e

} KAPC, *PKAPC, **PPKAPC ;

typedef struct _KDPC { // 0x20 bytes
    int16_t Type ; // offset 0x0
    uint8_t Number ; // offset 0x2
    uint8_t Importance ; // offset 0x3
    LIST_ENTRY DpcListEntry ; // offset 0x4
    uint32_t DeferredRoutine ; // offset 0xc
    uint32_t DeferredContext ; // offset 0x10
    uint32_t SystemArgument1 ; // offset 0x14
    uint32_t SystemArgument2 ; // offset 0x18
    uint32_t Lock ; // offset 0x1c

} KDPC, *PKDPC, **PPKDPC ;

typedef struct _ETIMER { // 0x98 bytes
    KTIMER KeTimer ; // offset 0x0
    KAPC TimerApc ; // offset 0x28
    KDPC TimerDpc ; // offset 0x58
    LIST_ENTRY ActiveTimerListEntry ; // offset 0x78
    uint32_t Lock ; // offset 0x80
    int32_t Period ; // offset 0x84
    uint8_t ApcAssociated ; // offset 0x88
    uint8_t WakeTimer ; // offset 0x89
    LIST_ENTRY WakeTimerListEntry ; // offset 0x8c

} ETIMER, *PETIMER, **PPETIMER ;

typedef struct _FILE_NETWORK_OPEN_INFORMATION { // 0x38 bytes
    LARGE_INTEGER CreationTime ; // offset 0x0
    LARGE_INTEGER LastAccessTime ; // offset 0x8
    LARGE_INTEGER LastWriteTime ; // offset 0x10
    LARGE_INTEGER ChangeTime ; // offset 0x18
    LARGE_INTEGER AllocationSize ; // offset 0x20
    LARGE_INTEGER EndOfFile ; // offset 0x28
    uint32_t FileAttributes ; // offset 0x30

} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION, **PPFILE_NETWORK_OPEN_INFORMATION ;

typedef struct _KSPIN_LOCK_QUEUE { // 0x8 bytes
    uint32_t Next ; // offset 0x0
    uint32_t Lock ; // offset 0x4

} KSPIN_LOCK_QUEUE, *PKSPIN_LOCK_QUEUE, **PPKSPIN_LOCK_QUEUE ;

typedef struct _KLOCK_QUEUE_HANDLE { // 0xc bytes
    KSPIN_LOCK_QUEUE LockQueue ; // offset 0x0
    uint8_t OldIrql ; // offset 0x8

} KLOCK_QUEUE_HANDLE, *PKLOCK_QUEUE_HANDLE, **PPKLOCK_QUEUE_HANDLE ;

typedef struct _SEGMENT_OBJECT { // 0x30 bytes
    uint32_t BaseAddress ; // offset 0x0
    uint32_t TotalNumberOfPtes ; // offset 0x4
    LARGE_INTEGER SizeOfSegment ; // offset 0x8
    uint32_t NonExtendedPtes ; // offset 0x10
    uint32_t ImageCommitment ; // offset 0x14
    uint32_t ControlArea ; // offset 0x18
    uint32_t Subsection ; // offset 0x1c
    uint32_t LargeControlArea ; // offset 0x20
    uint32_t MmSectionFlags ; // offset 0x24
    uint32_t MmSubSectionFlags ; // offset 0x28

} SEGMENT_OBJECT, *PSEGMENT_OBJECT, **PPSEGMENT_OBJECT ;

typedef struct _HIVE_LIST_ENTRY { // 0x18 bytes
    uint32_t Name ; // offset 0x0
    uint32_t BaseName ; // offset 0x4
    uint32_t CmHive ; // offset 0x8
    uint32_t Flags ; // offset 0xc
    uint32_t CmHive2 ; // offset 0x10
    uint8_t ThreadFinished ; // offset 0x14
    uint8_t ThreadStarted ; // offset 0x15
    uint8_t Allocate ; // offset 0x16

} HIVE_LIST_ENTRY, *PHIVE_LIST_ENTRY, **PPHIVE_LIST_ENTRY ;

typedef struct _PO_MEMORY_RANGE_ARRAY { // 0x10 bytes
    union {
    struct {
            uint32_t PageNo ; // offset 0x0
            uint32_t StartPage ; // offset 0x4
            uint32_t EndPage ; // offset 0x8
            uint32_t CheckSum ; // offset 0xc
        } Range ; // offset 0x0
    struct {
            uint32_t Next ; // offset 0x0
            uint32_t NextTable ; // offset 0x4
            uint32_t CheckSum ; // offset 0x8
            uint32_t EntryCount ; // offset 0xc
        } Link ; // offset 0x0
    };

} PO_MEMORY_RANGE_ARRAY, *PPO_MEMORY_RANGE_ARRAY, **PPPO_MEMORY_RANGE_ARRAY ;

typedef struct _IO_STATUS_BLOCK { // 0x8 bytes
    union {
    int32_t Status ; // offset 0x0
    uint32_t Pointer ; // offset 0x0
    };
    uint32_t Information ; // offset 0x4

} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK, **PPIO_STATUS_BLOCK ;

typedef struct _MDL { // 0x1c bytes
    uint32_t Next ; // offset 0x0
    int16_t Size ; // offset 0x4
    int16_t MdlFlags ; // offset 0x6
    uint32_t Process ; // offset 0x8
    uint32_t MappedSystemVa ; // offset 0xc
    uint32_t StartVa ; // offset 0x10
    uint32_t ByteCount ; // offset 0x14
    uint32_t ByteOffset ; // offset 0x18

} MDL, *PMDL, **PPMDL ;

typedef struct _MMMOD_WRITER_MDL_ENTRY { // 0x58 bytes
    LIST_ENTRY Links ; // offset 0x0
    LARGE_INTEGER WriteOffset ; // offset 0x8
    union {
        union {
        IO_STATUS_BLOCK IoStatus ; // offset 0x0
        LARGE_INTEGER LastByte ; // offset 0x0
        };
    } u ; // offset 0x10
    uint32_t Irp ; // offset 0x18
    uint32_t LastPageToWrite ; // offset 0x1c
    uint32_t PagingListHead ; // offset 0x20
    uint32_t CurrentList ; // offset 0x24
    uint32_t PagingFile ; // offset 0x28
    uint32_t File ; // offset 0x2c
    uint32_t ControlArea ; // offset 0x30
    uint32_t FileResource ; // offset 0x34
    MDL Mdl ; // offset 0x38
    uint32_t Page[1] ; // offset 0x54

} MMMOD_WRITER_MDL_ENTRY, *PMMMOD_WRITER_MDL_ENTRY, **PPMMMOD_WRITER_MDL_ENTRY ;

typedef struct _MMPAGING_FILE { // 0x44 bytes
    uint32_t Size ; // offset 0x0
    uint32_t MaximumSize ; // offset 0x4
    uint32_t MinimumSize ; // offset 0x8
    uint32_t FreeSpace ; // offset 0xc
    uint32_t CurrentUsage ; // offset 0x10
    uint32_t PeakUsage ; // offset 0x14
    uint32_t Hint ; // offset 0x18
    uint32_t HighestPage ; // offset 0x1c
    uint32_t Entry[2] ; // offset 0x20
    uint32_t Bitmap ; // offset 0x28
    uint32_t File ; // offset 0x2c
    UNICODE_STRING PageFileName ; // offset 0x30
    uint32_t PageFileNumber ; // offset 0x38
    uint8_t Extended ; // offset 0x3c
    uint8_t HintSetToZero ; // offset 0x3d
    uint8_t BootPartition ; // offset 0x3e
    uint32_t FileHandle ; // offset 0x40

} MMPAGING_FILE, *PMMPAGING_FILE, **PPMMPAGING_FILE ;

typedef struct _CM_CELL_REMAP_BLOCK { // 0x8 bytes
    uint32_t OldCell ; // offset 0x0
    uint32_t NewCell ; // offset 0x4

} CM_CELL_REMAP_BLOCK, *PCM_CELL_REMAP_BLOCK, **PPCM_CELL_REMAP_BLOCK ;

typedef struct _POP_SHUTDOWN_BUG_CHECK { // 0x14 bytes
    uint32_t Code ; // offset 0x0
    uint32_t Parameter1 ; // offset 0x4
    uint32_t Parameter2 ; // offset 0x8
    uint32_t Parameter3 ; // offset 0xc
    uint32_t Parameter4 ; // offset 0x10

} POP_SHUTDOWN_BUG_CHECK, *PPOP_SHUTDOWN_BUG_CHECK, **PPPOP_SHUTDOWN_BUG_CHECK ;

typedef struct _VI_DEADLOCK_RESOURCE { // 0x80 bytes
    VI_DEADLOCK_RESOURCE_TYPE Type ; // offset 0x0
    union {
    uint32_t NodeCount : 16 ; // offset 0x4
    uint32_t RecursionCount : 16 ; // offset 0x4
    };
    uint32_t ResourceAddress ; // offset 0x8
    uint32_t ThreadOwner ; // offset 0xc
    LIST_ENTRY ResourceList ; // offset 0x10
    union {
    LIST_ENTRY HashChainList ; // offset 0x18
    LIST_ENTRY FreeListEntry ; // offset 0x18
    };
    uint32_t StackTrace[8] ; // offset 0x20
    uint32_t LastAcquireTrace[8] ; // offset 0x40
    uint32_t LastReleaseTrace[8] ; // offset 0x60

} VI_DEADLOCK_RESOURCE, *PVI_DEADLOCK_RESOURCE, **PPVI_DEADLOCK_RESOURCE ;

typedef struct _KSEMAPHORE { // 0x14 bytes
    DISPATCHER_HEADER Header ; // offset 0x0
    int32_t Limit ; // offset 0x10

} KSEMAPHORE, *PKSEMAPHORE, **PPKSEMAPHORE ;

typedef struct _LPCP_NONPAGED_PORT_QUEUE { // 0x18 bytes
    KSEMAPHORE Semaphore ; // offset 0x0
    uint32_t BackPointer ; // offset 0x14

} LPCP_NONPAGED_PORT_QUEUE, *PLPCP_NONPAGED_PORT_QUEUE, **PPLPCP_NONPAGED_PORT_QUEUE ;

typedef struct _HEAP_ENTRY { // 0x8 bytes
    uint16_t Size ; // offset 0x0
    uint16_t PreviousSize ; // offset 0x2
    uint32_t SubSegmentCode ; // offset 0x0
    uint8_t SmallTagIndex ; // offset 0x4
    uint8_t Flags ; // offset 0x5
    uint8_t UnusedBytes ; // offset 0x6
    volatile uint8_t SegmentIndex ; // offset 0x7

} HEAP_ENTRY, *PHEAP_ENTRY, **PPHEAP_ENTRY ;

typedef struct _HEAP_SEGMENT { // 0x3c bytes
    HEAP_ENTRY Entry ; // offset 0x0
    uint32_t Signature ; // offset 0x8
    uint32_t Flags ; // offset 0xc
    uint32_t Heap ; // offset 0x10
    uint32_t LargestUnCommittedRange ; // offset 0x14
    uint32_t BaseAddress ; // offset 0x18
    uint32_t NumberOfPages ; // offset 0x1c
    uint32_t FirstEntry ; // offset 0x20
    uint32_t LastValidEntry ; // offset 0x24
    uint32_t NumberOfUnCommittedPages ; // offset 0x28
    uint32_t NumberOfUnCommittedRanges ; // offset 0x2c
    uint32_t UnCommittedRanges ; // offset 0x30
    uint16_t AllocatorBackTraceIndex ; // offset 0x34
    uint16_t Reserved ; // offset 0x36
    uint32_t LastEntryInSegment ; // offset 0x38

} HEAP_SEGMENT, *PHEAP_SEGMENT, **PPHEAP_SEGMENT ;

typedef struct _HEAP { // 0x588 bytes
    HEAP_ENTRY Entry ; // offset 0x0
    uint32_t Signature ; // offset 0x8
    uint32_t Flags ; // offset 0xc
    uint32_t ForceFlags ; // offset 0x10
    uint32_t VirtualMemoryThreshold ; // offset 0x14
    uint32_t SegmentReserve ; // offset 0x18
    uint32_t SegmentCommit ; // offset 0x1c
    uint32_t DeCommitFreeBlockThreshold ; // offset 0x20
    uint32_t DeCommitTotalFreeThreshold ; // offset 0x24
    uint32_t TotalFreeSize ; // offset 0x28
    uint32_t MaximumAllocationSize ; // offset 0x2c
    uint16_t ProcessHeapsListIndex ; // offset 0x30
    uint16_t HeaderValidateLength ; // offset 0x32
    uint32_t HeaderValidateCopy ; // offset 0x34
    uint16_t NextAvailableTagIndex ; // offset 0x38
    uint16_t MaximumTagIndex ; // offset 0x3a
    uint32_t TagEntries ; // offset 0x3c
    uint32_t UCRSegments ; // offset 0x40
    uint32_t UnusedUnCommittedRanges ; // offset 0x44
    uint32_t AlignRound ; // offset 0x48
    uint32_t AlignMask ; // offset 0x4c
    LIST_ENTRY VirtualAllocdBlocks ; // offset 0x50
    uint32_t Segments[64] ; // offset 0x58
    union {
        union {
        uint32_t FreeListsInUseUlong[4] ; // offset 0x0
        uint8_t FreeListsInUseBytes[16] ; // offset 0x0
        };
    } u ; // offset 0x158
    union {
        union {
        uint16_t FreeListsInUseTerminate ; // offset 0x0
        uint16_t DecommitCount ; // offset 0x0
        };
    } u2 ; // offset 0x168
    uint16_t AllocatorBackTraceIndex ; // offset 0x16a
    uint32_t NonDedicatedListLength ; // offset 0x16c
    uint32_t LargeBlocksIndex ; // offset 0x170
    uint32_t PseudoTagEntries ; // offset 0x174
    LIST_ENTRY FreeLists[128] ; // offset 0x178
    uint32_t LockVariable ; // offset 0x578
    uint32_t CommitRoutine ; // offset 0x57c
    uint32_t FrontEndHeap ; // offset 0x580
    uint16_t FrontHeapLockCount ; // offset 0x584
    uint8_t FrontEndHeapType ; // offset 0x586
    uint8_t LastSegmentIndex ; // offset 0x587

} HEAP, *PHEAP, **PPHEAP ;

typedef struct _KQUEUE { // 0x28 bytes
    DISPATCHER_HEADER Header ; // offset 0x0
    LIST_ENTRY EntryListHead ; // offset 0x10
    uint32_t CurrentCount ; // offset 0x18
    uint32_t MaximumCount ; // offset 0x1c
    LIST_ENTRY ThreadListHead ; // offset 0x20

} KQUEUE, *PKQUEUE, **PPKQUEUE ;

typedef union _EX_QUEUE_WORKER_INFO { // 0x4 bytes
    union {
    uint32_t QueueDisabled : 1 ; // offset 0x0
    uint32_t MakeThreadsAsNecessary : 1 ; // offset 0x0
    uint32_t WaitMode : 1 ; // offset 0x0
    uint32_t WorkerCount : 29 ; // offset 0x0
    int32_t QueueWorkerInfo ; // offset 0x0
    };

} EX_QUEUE_WORKER_INFO, *PEX_QUEUE_WORKER_INFO, **PPEX_QUEUE_WORKER_INFO ;

typedef struct _EX_WORK_QUEUE { // 0x3c bytes
    KQUEUE WorkerQueue ; // offset 0x0
    uint32_t DynamicThreadCount ; // offset 0x28
    uint32_t WorkItemsProcessed ; // offset 0x2c
    uint32_t WorkItemsProcessedLastPass ; // offset 0x30
    uint32_t QueueDepthLastPass ; // offset 0x34
    EX_QUEUE_WORKER_INFO Info ; // offset 0x38

} EX_WORK_QUEUE, *PEX_WORK_QUEUE, **PPEX_WORK_QUEUE ;

typedef struct _DEFERRED_WRITE { // 0x28 bytes
    int16_t NodeTypeCode ; // offset 0x0
    int16_t NodeByteSize ; // offset 0x2
    uint32_t FileObject ; // offset 0x4
    uint32_t BytesToWrite ; // offset 0x8
    LIST_ENTRY DeferredWriteLinks ; // offset 0xc
    uint32_t Event ; // offset 0x14
    uint32_t PostRoutine ; // offset 0x18
    uint32_t Context1 ; // offset 0x1c
    uint32_t Context2 ; // offset 0x20
    uint8_t LimitModifiedPages ; // offset 0x24

} DEFERRED_WRITE, *PDEFERRED_WRITE, **PPDEFERRED_WRITE ;

typedef struct _KTRAP_FRAME { // 0x8c bytes
    uint32_t DbgEbp ; // offset 0x0
    uint32_t DbgEip ; // offset 0x4
    uint32_t DbgArgMark ; // offset 0x8
    uint32_t DbgArgPointer ; // offset 0xc
    uint32_t TempSegCs ; // offset 0x10
    uint32_t TempEsp ; // offset 0x14
    uint32_t Dr0 ; // offset 0x18
    uint32_t Dr1 ; // offset 0x1c
    uint32_t Dr2 ; // offset 0x20
    uint32_t Dr3 ; // offset 0x24
    uint32_t Dr6 ; // offset 0x28
    uint32_t Dr7 ; // offset 0x2c
    uint32_t SegGs ; // offset 0x30
    uint32_t SegEs ; // offset 0x34
    uint32_t SegDs ; // offset 0x38
    uint32_t Edx ; // offset 0x3c
    uint32_t Ecx ; // offset 0x40
    uint32_t Eax ; // offset 0x44
    uint32_t PreviousPreviousMode ; // offset 0x48
    uint32_t ExceptionList ; // offset 0x4c
    uint32_t SegFs ; // offset 0x50
    uint32_t Edi ; // offset 0x54
    uint32_t Esi ; // offset 0x58
    uint32_t Ebx ; // offset 0x5c
    uint32_t Ebp ; // offset 0x60
    uint32_t ErrCode ; // offset 0x64
    uint32_t Eip ; // offset 0x68
    uint32_t SegCs ; // offset 0x6c
    uint32_t EFlags ; // offset 0x70
    uint32_t HardwareEsp ; // offset 0x74
    uint32_t HardwareSegSs ; // offset 0x78
    uint32_t V86Es ; // offset 0x7c
    uint32_t V86Ds ; // offset 0x80
    uint32_t V86Fs ; // offset 0x84
    uint32_t V86Gs ; // offset 0x88

} KTRAP_FRAME, *PKTRAP_FRAME, **PPKTRAP_FRAME ;

typedef struct _CM_INDEX_HINT_BLOCK { // 0x8 bytes
    uint32_t Count ; // offset 0x0
    uint32_t HashKey[1] ; // offset 0x4

} CM_INDEX_HINT_BLOCK, *PCM_INDEX_HINT_BLOCK, **PPCM_INDEX_HINT_BLOCK ;

typedef struct _SID_IDENTIFIER_AUTHORITY { // 0x6 bytes
    uint8_t Value[6] ; // offset 0x0

} SID_IDENTIFIER_AUTHORITY, *PSID_IDENTIFIER_AUTHORITY, **PPSID_IDENTIFIER_AUTHORITY ;

typedef struct _SID { // 0xc bytes
    uint8_t Revision ; // offset 0x0
    uint8_t SubAuthorityCount ; // offset 0x1
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority ; // offset 0x2
    uint32_t SubAuthority[1] ; // offset 0x8

} SID, *PSID, **PPSID ;

typedef struct _FILE_GET_QUOTA_INFORMATION { // 0x14 bytes
    uint32_t NextEntryOffset ; // offset 0x0
    uint32_t SidLength ; // offset 0x4
    SID Sid ; // offset 0x8

} FILE_GET_QUOTA_INFORMATION, *PFILE_GET_QUOTA_INFORMATION, **PPFILE_GET_QUOTA_INFORMATION ;

typedef struct _EXCEPTION_REGISTRATION_RECORD { // 0x8 bytes
    uint32_t Next ; // offset 0x0
    uint32_t Handler ; // offset 0x4

} EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD, **PPEXCEPTION_REGISTRATION_RECORD ;

typedef struct _DBGKD_CONTINUE { // 0x4 bytes
    int32_t ContinueStatus ; // offset 0x0

} DBGKD_CONTINUE, *PDBGKD_CONTINUE, **PPDBGKD_CONTINUE ;

typedef struct _DBGKD_SET_CONTEXT { // 0x4 bytes
    uint32_t ContextFlags ; // offset 0x0

} DBGKD_SET_CONTEXT, *PDBGKD_SET_CONTEXT, **PPDBGKD_SET_CONTEXT ;

typedef struct _DBGKD_SEARCH_MEMORY { // 0x18 bytes
    union {
    uint64_t SearchAddress ; // offset 0x0
    uint64_t FoundAddress ; // offset 0x0
    };
    uint64_t SearchLength ; // offset 0x8
    uint32_t PatternLength ; // offset 0x10

} DBGKD_SEARCH_MEMORY, *PDBGKD_SEARCH_MEMORY, **PPDBGKD_SEARCH_MEMORY ;

typedef struct _DBGKD_WRITE_BREAKPOINT64 { // 0x10 bytes
    uint64_t BreakPointAddress ; // offset 0x0
    uint32_t BreakPointHandle ; // offset 0x8

} DBGKD_WRITE_BREAKPOINT64, *PDBGKD_WRITE_BREAKPOINT64, **PPDBGKD_WRITE_BREAKPOINT64 ;

typedef struct _DBGKD_READ_MEMORY64 { // 0x10 bytes
    uint64_t TargetBaseAddress ; // offset 0x0
    uint32_t TransferCount ; // offset 0x8
    uint32_t ActualBytesRead ; // offset 0xc

} DBGKD_READ_MEMORY64, *PDBGKD_READ_MEMORY64, **PPDBGKD_READ_MEMORY64 ;

typedef struct _DBGKD_GET_CONTEXT { // 0x4 bytes
    uint32_t Unused ; // offset 0x0

} DBGKD_GET_CONTEXT, *PDBGKD_GET_CONTEXT, **PPDBGKD_GET_CONTEXT ;

typedef struct _DBGKD_QUERY_SPECIAL_CALLS { // 0x4 bytes
    uint32_t NumberOfSpecialCalls ; // offset 0x0

} DBGKD_QUERY_SPECIAL_CALLS, *PDBGKD_QUERY_SPECIAL_CALLS, **PPDBGKD_QUERY_SPECIAL_CALLS ;

typedef struct _DBGKD_BREAKPOINTEX { // 0x8 bytes
    uint32_t BreakPointCount ; // offset 0x0
    int32_t ContinueStatus ; // offset 0x4

} DBGKD_BREAKPOINTEX, *PDBGKD_BREAKPOINTEX, **PPDBGKD_BREAKPOINTEX ;

typedef struct _DBGKD_GET_INTERNAL_BREAKPOINT64 { // 0x20 bytes
    uint64_t BreakpointAddress ; // offset 0x0
    uint32_t Flags ; // offset 0x8
    uint32_t Calls ; // offset 0xc
    uint32_t MaxCallsPerPeriod ; // offset 0x10
    uint32_t MinInstructions ; // offset 0x14
    uint32_t MaxInstructions ; // offset 0x18
    uint32_t TotalInstructions ; // offset 0x1c

} DBGKD_GET_INTERNAL_BREAKPOINT64, *PDBGKD_GET_INTERNAL_BREAKPOINT64, **PPDBGKD_GET_INTERNAL_BREAKPOINT64 ;

typedef struct _DBGKD_RESTORE_BREAKPOINT { // 0x4 bytes
    uint32_t BreakPointHandle ; // offset 0x0

} DBGKD_RESTORE_BREAKPOINT, *PDBGKD_RESTORE_BREAKPOINT, **PPDBGKD_RESTORE_BREAKPOINT ;

typedef struct _DBGKD_READ_WRITE_MSR { // 0xc bytes
    uint32_t Msr ; // offset 0x0
    uint32_t DataValueLow ; // offset 0x4
    uint32_t DataValueHigh ; // offset 0x8

} DBGKD_READ_WRITE_MSR, *PDBGKD_READ_WRITE_MSR, **PPDBGKD_READ_WRITE_MSR ;

typedef struct _DBGKD_WRITE_MEMORY64 { // 0x10 bytes
    uint64_t TargetBaseAddress ; // offset 0x0
    uint32_t TransferCount ; // offset 0x8
    uint32_t ActualBytesWritten ; // offset 0xc

} DBGKD_WRITE_MEMORY64, *PDBGKD_WRITE_MEMORY64, **PPDBGKD_WRITE_MEMORY64 ;

typedef struct _DBGKD_SET_INTERNAL_BREAKPOINT64 { // 0x10 bytes
    uint64_t BreakpointAddress ; // offset 0x0
    uint32_t Flags ; // offset 0x8

} DBGKD_SET_INTERNAL_BREAKPOINT64, *PDBGKD_SET_INTERNAL_BREAKPOINT64, **PPDBGKD_SET_INTERNAL_BREAKPOINT64 ;

typedef struct _DBGKD_QUERY_MEMORY { // 0x18 bytes
    uint64_t Address ; // offset 0x0
    uint64_t Reserved ; // offset 0x8
    uint32_t AddressSpace ; // offset 0x10
    uint32_t Flags ; // offset 0x14

} DBGKD_QUERY_MEMORY, *PDBGKD_QUERY_MEMORY, **PPDBGKD_QUERY_MEMORY ;

typedef struct _DBGKD_GET_SET_BUS_DATA { // 0x14 bytes
    uint32_t BusDataType ; // offset 0x0
    uint32_t BusNumber ; // offset 0x4
    uint32_t SlotNumber ; // offset 0x8
    uint32_t Offset ; // offset 0xc
    uint32_t Length ; // offset 0x10

} DBGKD_GET_SET_BUS_DATA, *PDBGKD_GET_SET_BUS_DATA, **PPDBGKD_GET_SET_BUS_DATA ;

typedef struct _DBGKD_GET_VERSION64 { // 0x28 bytes
    uint16_t MajorVersion ; // offset 0x0
    uint16_t MinorVersion ; // offset 0x2
    uint16_t ProtocolVersion ; // offset 0x4
    uint16_t Flags ; // offset 0x6
    uint16_t MachineType ; // offset 0x8
    uint8_t MaxPacketType ; // offset 0xa
    uint8_t MaxStateChange ; // offset 0xb
    uint8_t MaxManipulate ; // offset 0xc
    uint8_t Simulation ; // offset 0xd
    uint16_t Unused[1] ; // offset 0xe
    uint64_t KernBase ; // offset 0x10
    uint64_t PsLoadedModuleList ; // offset 0x18
    uint64_t DebuggerDataList ; // offset 0x20

} DBGKD_GET_VERSION64, *PDBGKD_GET_VERSION64, **PPDBGKD_GET_VERSION64 ;

typedef struct _DBGKD_READ_WRITE_IO_EXTENDED64 { // 0x20 bytes
    uint32_t DataSize ; // offset 0x0
    uint32_t InterfaceType ; // offset 0x4
    uint32_t BusNumber ; // offset 0x8
    uint32_t AddressSpace ; // offset 0xc
    uint64_t IoAddress ; // offset 0x10
    uint32_t DataValue ; // offset 0x18

} DBGKD_READ_WRITE_IO_EXTENDED64, *PDBGKD_READ_WRITE_IO_EXTENDED64, **PPDBGKD_READ_WRITE_IO_EXTENDED64 ;

typedef struct _DBGKD_READ_WRITE_IO64 { // 0x10 bytes
    uint64_t IoAddress ; // offset 0x0
    uint32_t DataSize ; // offset 0x8
    uint32_t DataValue ; // offset 0xc

} DBGKD_READ_WRITE_IO64, *PDBGKD_READ_WRITE_IO64, **PPDBGKD_READ_WRITE_IO64 ;

typedef struct _X86_DBGKD_CONTROL_SET { // 0x10 bytes
    uint32_t TraceFlag ; // offset 0x0
    uint32_t Dr7 ; // offset 0x4
    uint32_t CurrentSymbolStart ; // offset 0x8
    uint32_t CurrentSymbolEnd ; // offset 0xc

} X86_DBGKD_CONTROL_SET, *PX86_DBGKD_CONTROL_SET, **PPX86_DBGKD_CONTROL_SET ;

typedef struct _IA64_DBGKD_CONTROL_SET { // 0x14 bytes
    uint32_t Continue ; // offset 0x0
    uint64_t CurrentSymbolStart ; // offset 0x4
    uint64_t CurrentSymbolEnd ; // offset 0xc

} IA64_DBGKD_CONTROL_SET, *PIA64_DBGKD_CONTROL_SET, **PPIA64_DBGKD_CONTROL_SET ;

typedef struct _AMD64_DBGKD_CONTROL_SET { // 0x1c bytes
    uint32_t TraceFlag ; // offset 0x0
    uint64_t Dr7 ; // offset 0x4
    uint64_t CurrentSymbolStart ; // offset 0xc
    uint64_t CurrentSymbolEnd ; // offset 0x14

} AMD64_DBGKD_CONTROL_SET, *PAMD64_DBGKD_CONTROL_SET, **PPAMD64_DBGKD_CONTROL_SET ;

typedef struct _DBGKD_ANY_CONTROL_SET { // 0x1c bytes
    union {
    X86_DBGKD_CONTROL_SET X86ControlSet ; // offset 0x0
    uint32_t AlphaControlSet ; // offset 0x0
    IA64_DBGKD_CONTROL_SET IA64ControlSet ; // offset 0x0
    AMD64_DBGKD_CONTROL_SET Amd64ControlSet ; // offset 0x0
    };

} DBGKD_ANY_CONTROL_SET, *PDBGKD_ANY_CONTROL_SET, **PPDBGKD_ANY_CONTROL_SET ;

typedef struct _DBGKD_CONTINUE2 { // 0x20 bytes
    int32_t ContinueStatus ; // offset 0x0
    union {
    X86_DBGKD_CONTROL_SET ControlSet ; // offset 0x4
    DBGKD_ANY_CONTROL_SET AnyControlSet ; // offset 0x4
    };

} DBGKD_CONTINUE2, *PDBGKD_CONTINUE2, **PPDBGKD_CONTINUE2 ;

typedef struct _DBGKD_SET_SPECIAL_CALL64 { // 0x8 bytes
    uint64_t SpecialCall ; // offset 0x0

} DBGKD_SET_SPECIAL_CALL64, *PDBGKD_SET_SPECIAL_CALL64, **PPDBGKD_SET_SPECIAL_CALL64 ;

typedef struct _DBGKD_FILL_MEMORY { // 0x10 bytes
    uint64_t Address ; // offset 0x0
    uint32_t Length ; // offset 0x8
    uint16_t Flags ; // offset 0xc
    uint16_t PatternLength ; // offset 0xe

} DBGKD_FILL_MEMORY, *PDBGKD_FILL_MEMORY, **PPDBGKD_FILL_MEMORY ;

typedef struct _DBGKD_MANIPULATE_STATE64 { // 0x38 bytes
    uint32_t ApiNumber ; // offset 0x0
    uint16_t ProcessorLevel ; // offset 0x4
    uint16_t Processor ; // offset 0x6
    int32_t ReturnStatus ; // offset 0x8
    union {
        union {
        DBGKD_READ_MEMORY64 ReadMemory ; // offset 0x0
        DBGKD_WRITE_MEMORY64 WriteMemory ; // offset 0x0
        DBGKD_GET_CONTEXT GetContext ; // offset 0x0
        DBGKD_SET_CONTEXT SetContext ; // offset 0x0
        DBGKD_WRITE_BREAKPOINT64 WriteBreakPoint ; // offset 0x0
        DBGKD_RESTORE_BREAKPOINT RestoreBreakPoint ; // offset 0x0
        DBGKD_CONTINUE Continue ; // offset 0x0
        DBGKD_CONTINUE2 Continue2 ; // offset 0x0
        DBGKD_READ_WRITE_IO64 ReadWriteIo ; // offset 0x0
        DBGKD_READ_WRITE_IO_EXTENDED64 ReadWriteIoExtended ; // offset 0x0
        DBGKD_QUERY_SPECIAL_CALLS QuerySpecialCalls ; // offset 0x0
        DBGKD_SET_SPECIAL_CALL64 SetSpecialCall ; // offset 0x0
        DBGKD_SET_INTERNAL_BREAKPOINT64 SetInternalBreakpoint ; // offset 0x0
        DBGKD_GET_INTERNAL_BREAKPOINT64 GetInternalBreakpoint ; // offset 0x0
        DBGKD_GET_VERSION64 GetVersion64 ; // offset 0x0
        DBGKD_BREAKPOINTEX BreakPointEx ; // offset 0x0
        DBGKD_READ_WRITE_MSR ReadWriteMsr ; // offset 0x0
        DBGKD_SEARCH_MEMORY SearchMemory ; // offset 0x0
        DBGKD_GET_SET_BUS_DATA GetSetBusData ; // offset 0x0
        DBGKD_FILL_MEMORY FillMemory ; // offset 0x0
        DBGKD_QUERY_MEMORY QueryMemory ; // offset 0x0
        };
    } u ; // offset 0x10

} DBGKD_MANIPULATE_STATE64, *PDBGKD_MANIPULATE_STATE64, **PPDBGKD_MANIPULATE_STATE64 ;

typedef struct _RTLP_RANGE_LIST_ENTRY { // 0x28 bytes
    uint64_t Start ; // offset 0x0
    uint64_t End ; // offset 0x8
    union {
    struct {
            uint32_t UserData ; // offset 0x0
            uint32_t Owner ; // offset 0x4
        } Allocated ; // offset 0x10
    struct {
            LIST_ENTRY ListHead ; // offset 0x0
        } Merged ; // offset 0x10
    };
    uint8_t Attributes ; // offset 0x18
    uint8_t PublicFlags ; // offset 0x19
    uint16_t PrivateFlags ; // offset 0x1a
    LIST_ENTRY ListEntry ; // offset 0x1c

} RTLP_RANGE_LIST_ENTRY, *PRTLP_RANGE_LIST_ENTRY, **PPRTLP_RANGE_LIST_ENTRY ;

typedef struct _IMAGE_FILE_HEADER { // 0x14 bytes
    uint16_t Machine ; // offset 0x0
    uint16_t NumberOfSections ; // offset 0x2
    uint32_t TimeDateStamp ; // offset 0x4
    uint32_t PointerToSymbolTable ; // offset 0x8
    uint32_t NumberOfSymbols ; // offset 0xc
    uint16_t SizeOfOptionalHeader ; // offset 0x10
    uint16_t Characteristics ; // offset 0x12

} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER, **PPIMAGE_FILE_HEADER ;

typedef struct _IMAGE_DATA_DIRECTORY { // 0x8 bytes
    uint32_t VirtualAddress ; // offset 0x0
    uint32_t Size ; // offset 0x4

} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY, **PPIMAGE_DATA_DIRECTORY ;

typedef struct _IMAGE_OPTIONAL_HEADER { // 0xe0 bytes
    uint16_t Magic ; // offset 0x0
    uint8_t MajorLinkerVersion ; // offset 0x2
    uint8_t MinorLinkerVersion ; // offset 0x3
    uint32_t SizeOfCode ; // offset 0x4
    uint32_t SizeOfInitializedData ; // offset 0x8
    uint32_t SizeOfUninitializedData ; // offset 0xc
    uint32_t AddressOfEntryPoint ; // offset 0x10
    uint32_t BaseOfCode ; // offset 0x14
    uint32_t BaseOfData ; // offset 0x18
    uint32_t ImageBase ; // offset 0x1c
    uint32_t SectionAlignment ; // offset 0x20
    uint32_t FileAlignment ; // offset 0x24
    uint16_t MajorOperatingSystemVersion ; // offset 0x28
    uint16_t MinorOperatingSystemVersion ; // offset 0x2a
    uint16_t MajorImageVersion ; // offset 0x2c
    uint16_t MinorImageVersion ; // offset 0x2e
    uint16_t MajorSubsystemVersion ; // offset 0x30
    uint16_t MinorSubsystemVersion ; // offset 0x32
    uint32_t Win32VersionValue ; // offset 0x34
    uint32_t SizeOfImage ; // offset 0x38
    uint32_t SizeOfHeaders ; // offset 0x3c
    uint32_t CheckSum ; // offset 0x40
    uint16_t Subsystem ; // offset 0x44
    uint16_t DllCharacteristics ; // offset 0x46
    uint32_t SizeOfStackReserve ; // offset 0x48
    uint32_t SizeOfStackCommit ; // offset 0x4c
    uint32_t SizeOfHeapReserve ; // offset 0x50
    uint32_t SizeOfHeapCommit ; // offset 0x54
    uint32_t LoaderFlags ; // offset 0x58
    uint32_t NumberOfRvaAndSizes ; // offset 0x5c
    IMAGE_DATA_DIRECTORY DataDirectory[16] ; // offset 0x60

} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER, **PPIMAGE_OPTIONAL_HEADER ;

typedef struct _IMAGE_NT_HEADERS { // 0xf8 bytes
    uint32_t Signature ; // offset 0x0
    IMAGE_FILE_HEADER FileHeader ; // offset 0x4
    IMAGE_OPTIONAL_HEADER OptionalHeader ; // offset 0x18

} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS, **PPIMAGE_NT_HEADERS ;

typedef struct _WORK_QUEUE_ITEM { // 0x10 bytes
    LIST_ENTRY List ; // offset 0x0
    uint32_t WorkerRoutine ; // offset 0x8
    uint32_t Parameter ; // offset 0xc

} WORK_QUEUE_ITEM, *PWORK_QUEUE_ITEM, **PPWORK_QUEUE_ITEM ;

typedef struct _SINGLE_LIST_ENTRY { // 0x4 bytes
    uint32_t Next ; // offset 0x0

} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY, **PPSINGLE_LIST_ENTRY ;

typedef struct _HEAP_USERDATA_HEADER { // 0x10 bytes
    union {
    SINGLE_LIST_ENTRY SFreeListEntry ; // offset 0x0
    uint32_t SubSegment ; // offset 0x0
    };
    uint32_t HeapHandle ; // offset 0x4
    uint32_t SizeIndex ; // offset 0x8
    uint32_t Signature ; // offset 0xc

} HEAP_USERDATA_HEADER, *PHEAP_USERDATA_HEADER, **PPHEAP_USERDATA_HEADER ;

typedef struct _HANDLE_TABLE { // 0x44 bytes
    uint32_t TableCode ; // offset 0x0
    uint32_t QuotaProcess ; // offset 0x4
    uint32_t UniqueProcessId ; // offset 0x8
    EX_PUSH_LOCK HandleTableLock[4] ; // offset 0xc
    LIST_ENTRY HandleTableList ; // offset 0x1c
    EX_PUSH_LOCK HandleContentionEvent ; // offset 0x24
    uint32_t DebugInfo ; // offset 0x28
    int32_t ExtraInfoPages ; // offset 0x2c
    uint32_t FirstFree ; // offset 0x30
    uint32_t LastFree ; // offset 0x34
    uint32_t NextHandleNeedingPool ; // offset 0x38
    int32_t HandleCount ; // offset 0x3c
    union {
    uint32_t Flags ; // offset 0x40
    uint8_t StrictFIFO : 1 ; // offset 0x40
    };

} HANDLE_TABLE, *PHANDLE_TABLE, **PPHANDLE_TABLE ;

typedef struct _MM_DRIVER_VERIFIER_DATA { // 0x70 bytes
    uint32_t Level ; // offset 0x0
    uint32_t RaiseIrqls ; // offset 0x4
    uint32_t AcquireSpinLocks ; // offset 0x8
    uint32_t SynchronizeExecutions ; // offset 0xc
    uint32_t AllocationsAttempted ; // offset 0x10
    uint32_t AllocationsSucceeded ; // offset 0x14
    uint32_t AllocationsSucceededSpecialPool ; // offset 0x18
    uint32_t AllocationsWithNoTag ; // offset 0x1c
    uint32_t TrimRequests ; // offset 0x20
    uint32_t Trims ; // offset 0x24
    uint32_t AllocationsFailed ; // offset 0x28
    uint32_t AllocationsFailedDeliberately ; // offset 0x2c
    uint32_t Loads ; // offset 0x30
    uint32_t Unloads ; // offset 0x34
    uint32_t UnTrackedPool ; // offset 0x38
    uint32_t UserTrims ; // offset 0x3c
    uint32_t CurrentPagedPoolAllocations ; // offset 0x40
    uint32_t CurrentNonPagedPoolAllocations ; // offset 0x44
    uint32_t PeakPagedPoolAllocations ; // offset 0x48
    uint32_t PeakNonPagedPoolAllocations ; // offset 0x4c
    uint32_t PagedBytes ; // offset 0x50
    uint32_t NonPagedBytes ; // offset 0x54
    uint32_t PeakPagedBytes ; // offset 0x58
    uint32_t PeakNonPagedBytes ; // offset 0x5c
    uint32_t BurstAllocationsFailedDeliberately ; // offset 0x60
    uint32_t SessionTrims ; // offset 0x64
    uint32_t Reserved[2] ; // offset 0x68

} MM_DRIVER_VERIFIER_DATA, *PMM_DRIVER_VERIFIER_DATA, **PPMM_DRIVER_VERIFIER_DATA ;

typedef struct _POP_POWER_ACTION { // 0x40 bytes
    uint8_t Updates ; // offset 0x0
    uint8_t State ; // offset 0x1
    uint8_t Shutdown ; // offset 0x2
    POWER_ACTION Action ; // offset 0x4
    SYSTEM_POWER_STATE LightestState ; // offset 0x8
    uint32_t Flags ; // offset 0xc
    int32_t Status ; // offset 0x10
    uint8_t IrpMinor ; // offset 0x14
    SYSTEM_POWER_STATE SystemState ; // offset 0x18
    SYSTEM_POWER_STATE NextSystemState ; // offset 0x1c
    uint32_t ShutdownBugCode ; // offset 0x20
    uint32_t DevState ; // offset 0x24
    uint32_t HiberContext ; // offset 0x28
    SYSTEM_POWER_STATE LastWakeState ; // offset 0x2c
    uint64_t WakeTime ; // offset 0x30
    uint64_t SleepTime ; // offset 0x38

} POP_POWER_ACTION, *PPOP_POWER_ACTION, **PPPOP_POWER_ACTION ;

typedef struct _CM_KEY_HASH { // 0x10 bytes
    uint32_t ConvKey ; // offset 0x0
    uint32_t NextHash ; // offset 0x4
    uint32_t KeyHive ; // offset 0x8
    uint32_t KeyCell ; // offset 0xc

} CM_KEY_HASH, *PCM_KEY_HASH, **PPCM_KEY_HASH ;

typedef struct _CACHED_CHILD_LIST { // 0x8 bytes
    uint32_t Count ; // offset 0x0
    union {
    uint32_t ValueList ; // offset 0x4
    uint32_t RealKcb ; // offset 0x4
    };

} CACHED_CHILD_LIST, *PCACHED_CHILD_LIST, **PPCACHED_CHILD_LIST ;

typedef struct _CM_KEY_CONTROL_BLOCK { // 0x48 bytes
    uint16_t RefCount ; // offset 0x0
    uint16_t Flags ; // offset 0x2
    union {
    uint32_t ExtFlags : 8 ; // offset 0x4
    uint32_t PrivateAlloc : 1 ; // offset 0x4
    uint32_t Delete : 1 ; // offset 0x4
    uint32_t DelayedCloseIndex : 12 ; // offset 0x4
    uint32_t TotalLevels : 10 ; // offset 0x4
    };
    union {
    CM_KEY_HASH KeyHash ; // offset 0x8
    uint32_t ConvKey ; // offset 0x8
    };
    uint32_t NextHash ; // offset 0xc
    uint32_t KeyHive ; // offset 0x10
    uint32_t KeyCell ; // offset 0x14
    uint32_t ParentKcb ; // offset 0x18
    uint32_t NameBlock ; // offset 0x1c
    uint32_t CachedSecurity ; // offset 0x20
    CACHED_CHILD_LIST ValueCache ; // offset 0x24
    union {
    uint32_t IndexHint ; // offset 0x2c
    uint32_t HashKey ; // offset 0x2c
    uint32_t SubKeyCount ; // offset 0x2c
    };
    union {
    LIST_ENTRY KeyBodyListHead ; // offset 0x30
    LIST_ENTRY FreeListEntry ; // offset 0x30
    };
    LARGE_INTEGER KcbLastWriteTime ; // offset 0x38
    uint16_t KcbMaxNameLen ; // offset 0x40
    uint16_t KcbMaxValueNameLen ; // offset 0x42
    uint32_t KcbMaxValueDataLen ; // offset 0x44

} CM_KEY_CONTROL_BLOCK, *PCM_KEY_CONTROL_BLOCK, **PPCM_KEY_CONTROL_BLOCK ;

typedef struct _IO_RESOURCE_DESCRIPTOR { // 0x20 bytes
    uint8_t Option ; // offset 0x0
    uint8_t Type ; // offset 0x1
    uint8_t ShareDisposition ; // offset 0x2
    uint8_t Spare1 ; // offset 0x3
    uint16_t Flags ; // offset 0x4
    uint16_t Spare2 ; // offset 0x6
    union {
        union {
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t Alignment ; // offset 0x4
                    LARGE_INTEGER MinimumAddress ; // offset 0x8
                    LARGE_INTEGER MaximumAddress ; // offset 0x10
                } Port ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t Alignment ; // offset 0x4
                    LARGE_INTEGER MinimumAddress ; // offset 0x8
                    LARGE_INTEGER MaximumAddress ; // offset 0x10
                } Memory ; // offset 0x0
        struct {
                    uint32_t MinimumVector ; // offset 0x0
                    uint32_t MaximumVector ; // offset 0x4
                } Interrupt ; // offset 0x0
        struct {
                    uint32_t MinimumChannel ; // offset 0x0
                    uint32_t MaximumChannel ; // offset 0x4
                } Dma ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t Alignment ; // offset 0x4
                    LARGE_INTEGER MinimumAddress ; // offset 0x8
                    LARGE_INTEGER MaximumAddress ; // offset 0x10
                } Generic ; // offset 0x0
        struct {
                    uint32_t Data[3] ; // offset 0x0
                } DevicePrivate ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t MinBusNumber ; // offset 0x4
                    uint32_t MaxBusNumber ; // offset 0x8
                    uint32_t Reserved ; // offset 0xc
                } BusNumber ; // offset 0x0
        struct {
                    uint32_t Priority ; // offset 0x0
                    uint32_t Reserved1 ; // offset 0x4
                    uint32_t Reserved2 ; // offset 0x8
                } ConfigData ; // offset 0x0
        };
    } u ; // offset 0x8

} IO_RESOURCE_DESCRIPTOR, *PIO_RESOURCE_DESCRIPTOR, **PPIO_RESOURCE_DESCRIPTOR ;

typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR { // 0x10 bytes
    uint8_t Type ; // offset 0x0
    uint8_t ShareDisposition ; // offset 0x1
    uint16_t Flags ; // offset 0x2
    union {
        union {
        struct {
                    LARGE_INTEGER Start ; // offset 0x0
                    uint32_t Length ; // offset 0x8
                } Generic ; // offset 0x0
        struct {
                    LARGE_INTEGER Start ; // offset 0x0
                    uint32_t Length ; // offset 0x8
                } Port ; // offset 0x0
        struct {
                    uint32_t Level ; // offset 0x0
                    uint32_t Vector ; // offset 0x4
                    uint32_t Affinity ; // offset 0x8
                } Interrupt ; // offset 0x0
        struct {
                    LARGE_INTEGER Start ; // offset 0x0
                    uint32_t Length ; // offset 0x8
                } Memory ; // offset 0x0
        struct {
                    uint32_t Channel ; // offset 0x0
                    uint32_t Port ; // offset 0x4
                    uint32_t Reserved1 ; // offset 0x8
                } Dma ; // offset 0x0
        struct {
                    uint32_t Data[3] ; // offset 0x0
                } DevicePrivate ; // offset 0x0
        struct {
                    uint32_t Start ; // offset 0x0
                    uint32_t Length ; // offset 0x4
                    uint32_t Reserved ; // offset 0x8
                } BusNumber ; // offset 0x0
        struct {
                    uint32_t DataSize ; // offset 0x0
                    uint32_t Reserved1 ; // offset 0x4
                    uint32_t Reserved2 ; // offset 0x8
                } DeviceSpecificData ; // offset 0x0
        };
    } u ; // offset 0x4

} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR, **PPCM_PARTIAL_RESOURCE_DESCRIPTOR ;

typedef struct _PCI_FUNCTION_RESOURCES { // 0x150 bytes
    IO_RESOURCE_DESCRIPTOR Limit[7] ; // offset 0x0
    CM_PARTIAL_RESOURCE_DESCRIPTOR Current[7] ; // offset 0xe0

} PCI_FUNCTION_RESOURCES, *PPCI_FUNCTION_RESOURCES, **PPPCI_FUNCTION_RESOURCES ;

typedef struct _MMVAD_FLAGS { // 0x4 bytes
    union {
    uint32_t CommitCharge : 19 ; // offset 0x0
    uint32_t PhysicalMapping : 1 ; // offset 0x0
    uint32_t ImageMap : 1 ; // offset 0x0
    uint32_t UserPhysicalPages : 1 ; // offset 0x0
    uint32_t NoChange : 1 ; // offset 0x0
    uint32_t WriteWatch : 1 ; // offset 0x0
    uint32_t Protection : 5 ; // offset 0x0
    uint32_t LargePages : 1 ; // offset 0x0
    uint32_t MemCommit : 1 ; // offset 0x0
    uint32_t PrivateMemory : 1 ; // offset 0x0
    };

} MMVAD_FLAGS, *PMMVAD_FLAGS, **PPMMVAD_FLAGS ;

typedef struct _MMVAD_FLAGS2 { // 0x4 bytes
    union {
    uint32_t FileOffset : 24 ; // offset 0x0
    uint32_t SecNoChange : 1 ; // offset 0x0
    uint32_t OneSecured : 1 ; // offset 0x0
    uint32_t MultipleSecured : 1 ; // offset 0x0
    uint32_t ReadOnly : 1 ; // offset 0x0
    uint32_t LongVad : 1 ; // offset 0x0
    uint32_t ExtendableFile : 1 ; // offset 0x0
    uint32_t Inherit : 1 ; // offset 0x0
    uint32_t CopyOnWrite : 1 ; // offset 0x0
    };

} MMVAD_FLAGS2, *PMMVAD_FLAGS2, **PPMMVAD_FLAGS2 ;

typedef struct _MMADDRESS_LIST { // 0x8 bytes
    uint32_t StartVpn ; // offset 0x0
    uint32_t EndVpn ; // offset 0x4

} MMADDRESS_LIST, *PMMADDRESS_LIST, **PPMMADDRESS_LIST ;

typedef struct _MMVAD_LONG { // 0x34 bytes
    uint32_t StartingVpn ; // offset 0x0
    uint32_t EndingVpn ; // offset 0x4
    uint32_t Parent ; // offset 0x8
    uint32_t LeftChild ; // offset 0xc
    uint32_t RightChild ; // offset 0x10
    union {
        union {
        uint32_t LongFlags ; // offset 0x0
        MMVAD_FLAGS VadFlags ; // offset 0x0
        };
    } u ; // offset 0x14
    uint32_t ControlArea ; // offset 0x18
    uint32_t FirstPrototypePte ; // offset 0x1c
    uint32_t LastContiguousPte ; // offset 0x20
    union {
        union {
        uint32_t LongFlags2 ; // offset 0x0
        MMVAD_FLAGS2 VadFlags2 ; // offset 0x0
        };
    } u2 ; // offset 0x24
    union {
        union {
        LIST_ENTRY List ; // offset 0x0
        MMADDRESS_LIST Secured ; // offset 0x0
        };
    } u3 ; // offset 0x28
    union {
        union {
        uint32_t Banked ; // offset 0x0
        uint32_t ExtendedInfo ; // offset 0x0
        };
    } u4 ; // offset 0x30

} MMVAD_LONG, *PMMVAD_LONG, **PPMMVAD_LONG ;

typedef struct _MMEXTEND_INFO { // 0x10 bytes
    uint64_t CommittedSize ; // offset 0x0
    uint32_t ReferenceCount ; // offset 0x8

} MMEXTEND_INFO, *PMMEXTEND_INFO, **PPMMEXTEND_INFO ;

typedef union _POWER_STATE { // 0x4 bytes
    union {
    SYSTEM_POWER_STATE SystemState ; // offset 0x0
    DEVICE_POWER_STATE DeviceState ; // offset 0x0
    };

} POWER_STATE, *PPOWER_STATE, **PPPOWER_STATE ;

typedef struct _IO_STACK_LOCATION { // 0x24 bytes
    uint8_t MajorFunction ; // offset 0x0
    uint8_t MinorFunction ; // offset 0x1
    uint8_t Flags ; // offset 0x2
    uint8_t Control ; // offset 0x3
    union {
        union {
        struct {
                    uint32_t SecurityContext ; // offset 0x0
                    uint32_t Options ; // offset 0x4
                    uint16_t FileAttributes ; // offset 0x8
                    uint16_t ShareAccess ; // offset 0xa
                    uint32_t EaLength ; // offset 0xc
                } Create ; // offset 0x0
        struct {
                    uint32_t SecurityContext ; // offset 0x0
                    uint32_t Options ; // offset 0x4
                    uint16_t Reserved ; // offset 0x8
                    uint16_t ShareAccess ; // offset 0xa
                    uint32_t Parameters ; // offset 0xc
                } CreatePipe ; // offset 0x0
        struct {
                    uint32_t SecurityContext ; // offset 0x0
                    uint32_t Options ; // offset 0x4
                    uint16_t Reserved ; // offset 0x8
                    uint16_t ShareAccess ; // offset 0xa
                    uint32_t Parameters ; // offset 0xc
                } CreateMailslot ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t Key ; // offset 0x4
                    LARGE_INTEGER ByteOffset ; // offset 0x8
                } Read ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t Key ; // offset 0x4
                    LARGE_INTEGER ByteOffset ; // offset 0x8
                } Write ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t FileName ; // offset 0x4
                    FILE_INFORMATION_CLASS FileInformationClass ; // offset 0x8
                    uint32_t FileIndex ; // offset 0xc
                } QueryDirectory ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t CompletionFilter ; // offset 0x4
                } NotifyDirectory ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    FILE_INFORMATION_CLASS FileInformationClass ; // offset 0x4
                } QueryFile ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    FILE_INFORMATION_CLASS FileInformationClass ; // offset 0x4
                    uint32_t FileObject ; // offset 0x8
                    uint8_t ReplaceIfExists ; // offset 0xc
                    uint8_t AdvanceOnly ; // offset 0xd
                    union {
                    uint32_t ClusterCount ; // offset 0xc
                    uint32_t DeleteHandle ; // offset 0xc
                    };
                } SetFile ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t EaList ; // offset 0x4
                    uint32_t EaListLength ; // offset 0x8
                    uint32_t EaIndex ; // offset 0xc
                } QueryEa ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                } SetEa ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    FSINFOCLASS FsInformationClass ; // offset 0x4
                } QueryVolume ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    FSINFOCLASS FsInformationClass ; // offset 0x4
                } SetVolume ; // offset 0x0
        struct {
                    uint32_t OutputBufferLength ; // offset 0x0
                    uint32_t InputBufferLength ; // offset 0x4
                    uint32_t FsControlCode ; // offset 0x8
                    uint32_t Type3InputBuffer ; // offset 0xc
                } FileSystemControl ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t Key ; // offset 0x4
                    LARGE_INTEGER ByteOffset ; // offset 0x8
                } LockControl ; // offset 0x0
        struct {
                    uint32_t OutputBufferLength ; // offset 0x0
                    uint32_t InputBufferLength ; // offset 0x4
                    uint32_t IoControlCode ; // offset 0x8
                    uint32_t Type3InputBuffer ; // offset 0xc
                } DeviceIoControl ; // offset 0x0
        struct {
                    uint32_t SecurityInformation ; // offset 0x0
                    uint32_t Length ; // offset 0x4
                } QuerySecurity ; // offset 0x0
        struct {
                    uint32_t SecurityInformation ; // offset 0x0
                    uint32_t SecurityDescriptor ; // offset 0x4
                } SetSecurity ; // offset 0x0
        struct {
                    uint32_t Vpb ; // offset 0x0
                    uint32_t DeviceObject ; // offset 0x4
                } MountVolume ; // offset 0x0
        struct {
                    uint32_t Vpb ; // offset 0x0
                    uint32_t DeviceObject ; // offset 0x4
                } VerifyVolume ; // offset 0x0
        struct {
                    uint32_t Srb ; // offset 0x0
                } Scsi ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                    uint32_t StartSid ; // offset 0x4
                    uint32_t SidList ; // offset 0x8
                    uint32_t SidListLength ; // offset 0xc
                } QueryQuota ; // offset 0x0
        struct {
                    uint32_t Length ; // offset 0x0
                } SetQuota ; // offset 0x0
        struct {
                    DEVICE_RELATION_TYPE Type ; // offset 0x0
                } QueryDeviceRelations ; // offset 0x0
        struct {
                    uint32_t InterfaceType ; // offset 0x0
                    uint16_t Size ; // offset 0x4
                    uint16_t Version ; // offset 0x6
                    uint32_t Interface ; // offset 0x8
                    uint32_t InterfaceSpecificData ; // offset 0xc
                } QueryInterface ; // offset 0x0
        struct {
                    uint32_t Capabilities ; // offset 0x0
                } DeviceCapabilities ; // offset 0x0
        struct {
                    uint32_t IoResourceRequirementList ; // offset 0x0
                } FilterResourceRequirements ; // offset 0x0
        struct {
                    uint32_t WhichSpace ; // offset 0x0
                    uint32_t Buffer ; // offset 0x4
                    uint32_t Offset ; // offset 0x8
                    uint32_t Length ; // offset 0xc
                } ReadWriteConfig ; // offset 0x0
        struct {
                    uint8_t Lock ; // offset 0x0
                } SetLock ; // offset 0x0
        struct {
                    BUS_QUERY_ID_TYPE IdType ; // offset 0x0
                } QueryId ; // offset 0x0
        struct {
                    DEVICE_TEXT_TYPE DeviceTextType ; // offset 0x0
                    uint32_t LocaleId ; // offset 0x4
                } QueryDeviceText ; // offset 0x0
        struct {
                    uint8_t InPath ; // offset 0x0
                    uint8_t Reserved[3] ; // offset 0x1
                    DEVICE_USAGE_NOTIFICATION_TYPE Type ; // offset 0x4
                } UsageNotification ; // offset 0x0
        struct {
                    SYSTEM_POWER_STATE PowerState ; // offset 0x0
                } WaitWake ; // offset 0x0
        struct {
                    uint32_t PowerSequence ; // offset 0x0
                } PowerSequence ; // offset 0x0
        struct {
                    uint32_t SystemContext ; // offset 0x0
                    POWER_STATE_TYPE Type ; // offset 0x4
                    POWER_STATE State ; // offset 0x8
                    POWER_ACTION ShutdownType ; // offset 0xc
                } Power ; // offset 0x0
        struct {
                    uint32_t AllocatedResources ; // offset 0x0
                    uint32_t AllocatedResourcesTranslated ; // offset 0x4
                } StartDevice ; // offset 0x0
        struct {
                    uint32_t ProviderId ; // offset 0x0
                    uint32_t DataPath ; // offset 0x4
                    uint32_t BufferSize ; // offset 0x8
                    uint32_t Buffer ; // offset 0xc
                } WMI ; // offset 0x0
        struct {
                    uint32_t Argument1 ; // offset 0x0
                    uint32_t Argument2 ; // offset 0x4
                    uint32_t Argument3 ; // offset 0x8
                    uint32_t Argument4 ; // offset 0xc
                } Others ; // offset 0x0
        };
    } Parameters ; // offset 0x4
    uint32_t DeviceObject ; // offset 0x14
    uint32_t FileObject ; // offset 0x18
    uint32_t CompletionRoutine ; // offset 0x1c
    uint32_t Context ; // offset 0x20

} IO_STACK_LOCATION, *PIO_STACK_LOCATION, **PPIO_STACK_LOCATION ;

typedef struct _SECTION_OBJECT_POINTERS { // 0xc bytes
    uint32_t DataSectionObject ; // offset 0x0
    uint32_t SharedCacheMap ; // offset 0x4
    uint32_t ImageSectionObject ; // offset 0x8

} SECTION_OBJECT_POINTERS, *PSECTION_OBJECT_POINTERS, **PPSECTION_OBJECT_POINTERS ;

typedef struct _PCI_MJ_DISPATCH_TABLE { // 0x20 bytes
    uint32_t PnpIrpMaximumMinorFunction ; // offset 0x0
    uint32_t PnpIrpDispatchTable ; // offset 0x4
    uint32_t PowerIrpMaximumMinorFunction ; // offset 0x8
    uint32_t PowerIrpDispatchTable ; // offset 0xc
    PCI_DISPATCH_STYLE SystemControlIrpDispatchStyle ; // offset 0x10
    uint32_t SystemControlIrpDispatchFunction ; // offset 0x14
    PCI_DISPATCH_STYLE OtherIrpDispatchStyle ; // offset 0x18
    uint32_t OtherIrpDispatchFunction ; // offset 0x1c

} PCI_MJ_DISPATCH_TABLE, *PPCI_MJ_DISPATCH_TABLE, **PPPCI_MJ_DISPATCH_TABLE ;

typedef struct _PI_BUS_EXTENSION { // 0x44 bytes
    uint32_t Flags ; // offset 0x0
    uint32_t NumberCSNs ; // offset 0x4
    uint32_t ReadDataPort ; // offset 0x8
    uint8_t DataPortMapped ; // offset 0xc
    uint32_t AddressPort ; // offset 0x10
    uint8_t AddrPortMapped ; // offset 0x14
    uint32_t CommandPort ; // offset 0x18
    uint8_t CmdPortMapped ; // offset 0x1c
    uint32_t NextSlotNumber ; // offset 0x20
    SINGLE_LIST_ENTRY DeviceList ; // offset 0x24
    SINGLE_LIST_ENTRY CardList ; // offset 0x28
    uint32_t PhysicalBusDevice ; // offset 0x2c
    uint32_t FunctionalBusDevice ; // offset 0x30
    uint32_t AttachedDevice ; // offset 0x34
    uint32_t BusNumber ; // offset 0x38
    SYSTEM_POWER_STATE SystemPowerState ; // offset 0x3c
    DEVICE_POWER_STATE DevicePowerState ; // offset 0x40

} PI_BUS_EXTENSION, *PPI_BUS_EXTENSION, **PPPI_BUS_EXTENSION ;

typedef struct _POOL_TRACKER_BIG_PAGES { // 0xc bytes
    uint32_t Va ; // offset 0x0
    uint32_t Key ; // offset 0x4
    uint32_t NumberOfPages ; // offset 0x8

} POOL_TRACKER_BIG_PAGES, *PPOOL_TRACKER_BIG_PAGES, **PPPOOL_TRACKER_BIG_PAGES ;

typedef struct _CM_NAME_HASH { // 0xc bytes
    uint32_t ConvKey ; // offset 0x0
    uint32_t NextHash ; // offset 0x4
    uint16_t NameLength ; // offset 0x8
    uint16_t Name[1] ; // offset 0xa

} CM_NAME_HASH, *PCM_NAME_HASH, **PPCM_NAME_HASH ;

typedef struct _CM_NAME_CONTROL_BLOCK { // 0x10 bytes
    uint8_t Compressed ; // offset 0x0
    uint16_t RefCount ; // offset 0x2
    union {
    CM_NAME_HASH NameHash ; // offset 0x4
    uint32_t ConvKey ; // offset 0x4
    };
    uint32_t NextHash ; // offset 0x8
    uint16_t NameLength ; // offset 0xc
    uint16_t Name[1] ; // offset 0xe

} CM_NAME_CONTROL_BLOCK, *PCM_NAME_CONTROL_BLOCK, **PPCM_NAME_CONTROL_BLOCK ;

typedef struct _SECURITY_DESCRIPTOR { // 0x14 bytes
    uint8_t Revision ; // offset 0x0
    uint8_t Sbz1 ; // offset 0x1
    uint16_t Control ; // offset 0x2
    uint32_t Owner ; // offset 0x4
    uint32_t Group ; // offset 0x8
    uint32_t Sacl ; // offset 0xc
    uint32_t Dacl ; // offset 0x10

} SECURITY_DESCRIPTOR, *PSECURITY_DESCRIPTOR, **PPSECURITY_DESCRIPTOR ;

typedef struct _SECURITY_TOKEN_AUDIT_DATA { // 0xc bytes
    uint32_t Length ; // offset 0x0
    uint32_t GrantMask ; // offset 0x4
    uint32_t DenyMask ; // offset 0x8

} SECURITY_TOKEN_AUDIT_DATA, *PSECURITY_TOKEN_AUDIT_DATA, **PPSECURITY_TOKEN_AUDIT_DATA ;

typedef struct _MMSUBSECTION_FLAGS { // 0x4 bytes
    union {
    uint32_t ReadOnly : 1 ; // offset 0x0
    uint32_t ReadWrite : 1 ; // offset 0x0
    uint32_t SubsectionStatic : 1 ; // offset 0x0
    uint32_t GlobalMemory : 1 ; // offset 0x0
    uint32_t Protection : 5 ; // offset 0x0
    uint32_t LargePages : 1 ; // offset 0x0
    uint32_t StartingSector4132 : 10 ; // offset 0x0
    uint32_t SectorEndOffset : 12 ; // offset 0x0
    };

} MMSUBSECTION_FLAGS, *PMMSUBSECTION_FLAGS, **PPMMSUBSECTION_FLAGS ;

typedef struct _SUBSECTION { // 0x20 bytes
    uint32_t ControlArea ; // offset 0x0
    union {
        union {
        uint32_t LongFlags ; // offset 0x0
        MMSUBSECTION_FLAGS SubsectionFlags ; // offset 0x0
        };
    } u ; // offset 0x4
    uint32_t StartingSector ; // offset 0x8
    uint32_t NumberOfFullSectors ; // offset 0xc
    uint32_t SubsectionBase ; // offset 0x10
    uint32_t UnusedPtes ; // offset 0x14
    uint32_t PtesInSubsection ; // offset 0x18
    uint32_t NextSubsection ; // offset 0x1c

} SUBSECTION, *PSUBSECTION, **PPSUBSECTION ;

typedef struct _CLIENT_ID { // 0x8 bytes
    uint32_t UniqueProcess ; // offset 0x0
    uint32_t UniqueThread ; // offset 0x4

} CLIENT_ID, *PCLIENT_ID, **PPCLIENT_ID ;

typedef struct _NT_TIB { // 0x1c bytes
    uint32_t ExceptionList ; // offset 0x0
    uint32_t StackBase ; // offset 0x4
    uint32_t StackLimit ; // offset 0x8
    uint32_t SubSystemTib ; // offset 0xc
    union {
    uint32_t FiberData ; // offset 0x10
    uint32_t Version ; // offset 0x10
    };
    uint32_t ArbitraryUserPointer ; // offset 0x14
    uint32_t Self ; // offset 0x18

} NT_TIB, *PNT_TIB, **PPNT_TIB ;

typedef struct _GDI_TEB_BATCH { // 0x4e0 bytes
    uint32_t Offset ; // offset 0x0
    uint32_t HDC ; // offset 0x4
    uint32_t Buffer[310] ; // offset 0x8

} GDI_TEB_BATCH, *PGDI_TEB_BATCH, **PPGDI_TEB_BATCH ;

typedef struct _ACTIVATION_CONTEXT_STACK { // 0x14 bytes
    uint32_t Flags ; // offset 0x0
    uint32_t NextCookieSequenceNumber ; // offset 0x4
    uint32_t ActiveFrame ; // offset 0x8
    LIST_ENTRY FrameListCache ; // offset 0xc

} ACTIVATION_CONTEXT_STACK, *PACTIVATION_CONTEXT_STACK, **PPACTIVATION_CONTEXT_STACK ;

typedef struct _Wx86ThreadState { // 0xc bytes
    uint32_t CallBx86Eip ; // offset 0x0
    uint32_t DeallocationCpu ; // offset 0x4
    uint8_t UseKnownWx86Dll ; // offset 0x8
    uint8_t OleStubInvoked ; // offset 0x9

} Wx86ThreadState, *PWx86ThreadState, **PPWx86ThreadState ;

typedef struct _TEB { // 0xfb8 bytes
    NT_TIB NtTib ; // offset 0x0
    uint32_t EnvironmentPointer ; // offset 0x1c
    CLIENT_ID ClientId ; // offset 0x20
    uint32_t ActiveRpcHandle ; // offset 0x28
    uint32_t ThreadLocalStoragePointer ; // offset 0x2c
    uint32_t ProcessEnvironmentBlock ; // offset 0x30
    uint32_t LastErrorValue ; // offset 0x34
    uint32_t CountOfOwnedCriticalSections ; // offset 0x38
    uint32_t CsrClientThread ; // offset 0x3c
    uint32_t Win32ThreadInfo ; // offset 0x40
    uint32_t User32Reserved[26] ; // offset 0x44
    uint32_t UserReserved[5] ; // offset 0xac
    uint32_t WOW32Reserved ; // offset 0xc0
    uint32_t CurrentLocale ; // offset 0xc4
    uint32_t FpSoftwareStatusRegister ; // offset 0xc8
    uint32_t SystemReserved1[54] ; // offset 0xcc
    int32_t ExceptionCode ; // offset 0x1a4
    ACTIVATION_CONTEXT_STACK ActivationContextStack ; // offset 0x1a8
    uint8_t SpareBytes1[24] ; // offset 0x1bc
    GDI_TEB_BATCH GdiTebBatch ; // offset 0x1d4
    CLIENT_ID RealClientId ; // offset 0x6b4
    uint32_t GdiCachedProcessHandle ; // offset 0x6bc
    uint32_t GdiClientPID ; // offset 0x6c0
    uint32_t GdiClientTID ; // offset 0x6c4
    uint32_t GdiThreadLocalInfo ; // offset 0x6c8
    uint32_t Win32ClientInfo[62] ; // offset 0x6cc
    uint32_t glDispatchTable[233] ; // offset 0x7c4
    uint32_t glReserved1[29] ; // offset 0xb68
    uint32_t glReserved2 ; // offset 0xbdc
    uint32_t glSectionInfo ; // offset 0xbe0
    uint32_t glSection ; // offset 0xbe4
    uint32_t glTable ; // offset 0xbe8
    uint32_t glCurrentRC ; // offset 0xbec
    uint32_t glContext ; // offset 0xbf0
    uint32_t LastStatusValue ; // offset 0xbf4
    UNICODE_STRING StaticUnicodeString ; // offset 0xbf8
    uint16_t StaticUnicodeBuffer[261] ; // offset 0xc00
    uint32_t DeallocationStack ; // offset 0xe0c
    uint32_t TlsSlots[64] ; // offset 0xe10
    LIST_ENTRY TlsLinks ; // offset 0xf10
    uint32_t Vdm ; // offset 0xf18
    uint32_t ReservedForNtRpc ; // offset 0xf1c
    uint32_t DbgSsReserved[2] ; // offset 0xf20
    uint32_t HardErrorsAreDisabled ; // offset 0xf28
    uint32_t Instrumentation[16] ; // offset 0xf2c
    uint32_t WinSockData ; // offset 0xf6c
    uint32_t GdiBatchCount ; // offset 0xf70
    uint8_t InDbgPrint ; // offset 0xf74
    uint8_t FreeStackOnTermination ; // offset 0xf75
    uint8_t HasFiberData ; // offset 0xf76
    uint8_t IdealProcessor ; // offset 0xf77
    uint32_t Spare3 ; // offset 0xf78
    uint32_t ReservedForPerf ; // offset 0xf7c
    uint32_t ReservedForOle ; // offset 0xf80
    uint32_t WaitingOnLoaderLock ; // offset 0xf84
    Wx86ThreadState Wx86Thread ; // offset 0xf88
    uint32_t TlsExpansionSlots ; // offset 0xf94
    uint32_t ImpersonationLocale ; // offset 0xf98
    uint32_t IsImpersonating ; // offset 0xf9c
    uint32_t NlsCache ; // offset 0xfa0
    uint32_t pShimData ; // offset 0xfa4
    uint32_t HeapVirtualAffinity ; // offset 0xfa8
    uint32_t CurrentTransactionHandle ; // offset 0xfac
    uint32_t ActiveFrame ; // offset 0xfb0
    uint8_t SafeThunkCall ; // offset 0xfb4
    uint8_t BooleanSpare[3] ; // offset 0xfb5

} TEB, *PTEB, **PPTEB ;

typedef struct _CURDIR { // 0xc bytes
    UNICODE_STRING DosPath ; // offset 0x0
    uint32_t Handle ; // offset 0x8

} CURDIR, *PCURDIR, **PPCURDIR ;

typedef struct _STRING { // 0x8 bytes
    uint16_t Length ; // offset 0x0
    uint16_t MaximumLength ; // offset 0x2
    uint32_t Buffer ; // offset 0x4

} STRING, *PSTRING, **PPSTRING ;

typedef struct _RTL_DRIVE_LETTER_CURDIR { // 0x10 bytes
    uint16_t Flags ; // offset 0x0
    uint16_t Length ; // offset 0x2
    uint32_t TimeStamp ; // offset 0x4
    STRING DosPath ; // offset 0x8

} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR, **PPRTL_DRIVE_LETTER_CURDIR ;

typedef struct _RTL_USER_PROCESS_PARAMETERS { // 0x290 bytes
    uint32_t MaximumLength ; // offset 0x0
    uint32_t Length ; // offset 0x4
    uint32_t Flags ; // offset 0x8
    uint32_t DebugFlags ; // offset 0xc
    uint32_t ConsoleHandle ; // offset 0x10
    uint32_t ConsoleFlags ; // offset 0x14
    uint32_t StandardInput ; // offset 0x18
    uint32_t StandardOutput ; // offset 0x1c
    uint32_t StandardError ; // offset 0x20
    CURDIR CurrentDirectory ; // offset 0x24
    UNICODE_STRING DllPath ; // offset 0x30
    UNICODE_STRING ImagePathName ; // offset 0x38
    UNICODE_STRING CommandLine ; // offset 0x40
    uint32_t Environment ; // offset 0x48
    uint32_t StartingX ; // offset 0x4c
    uint32_t StartingY ; // offset 0x50
    uint32_t CountX ; // offset 0x54
    uint32_t CountY ; // offset 0x58
    uint32_t CountCharsX ; // offset 0x5c
    uint32_t CountCharsY ; // offset 0x60
    uint32_t FillAttribute ; // offset 0x64
    uint32_t WindowFlags ; // offset 0x68
    uint32_t ShowWindowFlags ; // offset 0x6c
    UNICODE_STRING WindowTitle ; // offset 0x70
    UNICODE_STRING DesktopInfo ; // offset 0x78
    UNICODE_STRING ShellInfo ; // offset 0x80
    UNICODE_STRING RuntimeData ; // offset 0x88
    RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32] ; // offset 0x90

} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS, **PPRTL_USER_PROCESS_PARAMETERS ;

typedef struct _IO_RESOURCE_LIST { // 0x28 bytes
    uint16_t Version ; // offset 0x0
    uint16_t Revision ; // offset 0x2
    uint32_t Count ; // offset 0x4
    IO_RESOURCE_DESCRIPTOR Descriptors[1] ; // offset 0x8

} IO_RESOURCE_LIST, *PIO_RESOURCE_LIST, **PPIO_RESOURCE_LIST ;

typedef struct _IO_RESOURCE_REQUIREMENTS_LIST { // 0x48 bytes
    uint32_t ListSize ; // offset 0x0
    INTERFACE_TYPE InterfaceType ; // offset 0x4
    uint32_t BusNumber ; // offset 0x8
    uint32_t SlotNumber ; // offset 0xc
    uint32_t Reserved[3] ; // offset 0x10
    uint32_t AlternativeLists ; // offset 0x1c
    IO_RESOURCE_LIST List[1] ; // offset 0x20

} IO_RESOURCE_REQUIREMENTS_LIST, *PIO_RESOURCE_REQUIREMENTS_LIST, **PPIO_RESOURCE_REQUIREMENTS_LIST ;

typedef struct _KDEVICE_QUEUE_ENTRY { // 0x10 bytes
    LIST_ENTRY DeviceListEntry ; // offset 0x0
    uint32_t SortKey ; // offset 0x8
    uint8_t Inserted ; // offset 0xc

} KDEVICE_QUEUE_ENTRY, *PKDEVICE_QUEUE_ENTRY, **PPKDEVICE_QUEUE_ENTRY ;

typedef struct _WAIT_CONTEXT_BLOCK { // 0x28 bytes
    KDEVICE_QUEUE_ENTRY WaitQueueEntry ; // offset 0x0
    uint32_t DeviceRoutine ; // offset 0x10
    uint32_t DeviceContext ; // offset 0x14
    uint32_t NumberOfMapRegisters ; // offset 0x18
    uint32_t DeviceObject ; // offset 0x1c
    uint32_t CurrentIrp ; // offset 0x20
    uint32_t BufferChainingDpc ; // offset 0x24

} WAIT_CONTEXT_BLOCK, *PWAIT_CONTEXT_BLOCK, **PPWAIT_CONTEXT_BLOCK ;

typedef struct _KDEVICE_QUEUE { // 0x14 bytes
    int16_t Type ; // offset 0x0
    int16_t Size ; // offset 0x2
    LIST_ENTRY DeviceListHead ; // offset 0x4
    uint32_t Lock ; // offset 0xc
    uint8_t Busy ; // offset 0x10

} KDEVICE_QUEUE, *PKDEVICE_QUEUE, **PPKDEVICE_QUEUE ;

typedef struct _KEVENT { // 0x10 bytes
    DISPATCHER_HEADER Header ; // offset 0x0

} KEVENT, *PKEVENT, **PPKEVENT ;

typedef struct _DEVICE_OBJECT { // 0xb8 bytes
    int16_t Type ; // offset 0x0
    uint16_t Size ; // offset 0x2
    int32_t ReferenceCount ; // offset 0x4
    uint32_t DriverObject ; // offset 0x8
    uint32_t NextDevice ; // offset 0xc
    uint32_t AttachedDevice ; // offset 0x10
    uint32_t CurrentIrp ; // offset 0x14
    uint32_t Timer ; // offset 0x18
    uint32_t Flags ; // offset 0x1c
    uint32_t Characteristics ; // offset 0x20
    uint32_t Vpb ; // offset 0x24
    uint32_t DeviceExtension ; // offset 0x28
    uint32_t DeviceType ; // offset 0x2c
    uint8_t StackSize ; // offset 0x30
    union {
        union {
        LIST_ENTRY ListEntry ; // offset 0x0
        WAIT_CONTEXT_BLOCK Wcb ; // offset 0x0
        };
    } Queue ; // offset 0x34
    uint32_t AlignmentRequirement ; // offset 0x5c
    KDEVICE_QUEUE DeviceQueue ; // offset 0x60
    KDPC Dpc ; // offset 0x74
    uint32_t ActiveThreadCount ; // offset 0x94
    uint32_t SecurityDescriptor ; // offset 0x98
    KEVENT DeviceLock ; // offset 0x9c
    uint16_t SectorSize ; // offset 0xac
    uint16_t Spare1 ; // offset 0xae
    uint32_t DeviceObjectExtension ; // offset 0xb0
    uint32_t Reserved ; // offset 0xb4

} DEVICE_OBJECT, *PDEVICE_OBJECT, **PPDEVICE_OBJECT ;

typedef struct _DEVICE_RELATIONS { // 0x8 bytes
    uint32_t Count ; // offset 0x0
    uint32_t Objects[1] ; // offset 0x4

} DEVICE_RELATIONS, *PDEVICE_RELATIONS, **PPDEVICE_RELATIONS ;

typedef struct _IO_TIMER { // 0x18 bytes
    int16_t Type ; // offset 0x0
    int16_t TimerFlag ; // offset 0x2
    LIST_ENTRY TimerList ; // offset 0x4
    uint32_t TimerRoutine ; // offset 0xc
    uint32_t Context ; // offset 0x10
    uint32_t DeviceObject ; // offset 0x14

} IO_TIMER, *PIO_TIMER, **PPIO_TIMER ;

typedef struct _DEVOBJ_EXTENSION { // 0x2c bytes
    int16_t Type ; // offset 0x0
    uint16_t Size ; // offset 0x2
    uint32_t DeviceObject ; // offset 0x4
    uint32_t PowerFlags ; // offset 0x8
    uint32_t Dope ; // offset 0xc
    uint32_t ExtensionFlags ; // offset 0x10
    uint32_t DeviceNode ; // offset 0x14
    uint32_t AttachedTo ; // offset 0x18
    int32_t StartIoCount ; // offset 0x1c
    int32_t StartIoKey ; // offset 0x20
    uint32_t StartIoFlags ; // offset 0x24
    uint32_t Vpb ; // offset 0x28

} DEVOBJ_EXTENSION, *PDEVOBJ_EXTENSION, **PPDEVOBJ_EXTENSION ;

typedef struct _POOL_TRACKER_TABLE { // 0x1c bytes
    uint32_t Key ; // offset 0x0
    uint32_t NonPagedAllocs ; // offset 0x4
    uint32_t NonPagedFrees ; // offset 0x8
    uint32_t NonPagedBytes ; // offset 0xc
    uint32_t PagedAllocs ; // offset 0x10
    uint32_t PagedFrees ; // offset 0x14
    uint32_t PagedBytes ; // offset 0x18

} POOL_TRACKER_TABLE, *PPOOL_TRACKER_TABLE, **PPPOOL_TRACKER_TABLE ;

typedef struct _LPCP_PORT_QUEUE { // 0x10 bytes
    uint32_t NonPagedPortQueue ; // offset 0x0
    uint32_t Semaphore ; // offset 0x4
    LIST_ENTRY ReceiveHead ; // offset 0x8

} LPCP_PORT_QUEUE, *PLPCP_PORT_QUEUE, **PPLPCP_PORT_QUEUE ;

typedef struct _SECURITY_QUALITY_OF_SERVICE { // 0xc bytes
    uint32_t Length ; // offset 0x0
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel ; // offset 0x4
    uint8_t ContextTrackingMode ; // offset 0x8
    uint8_t EffectiveOnly ; // offset 0x9

} SECURITY_QUALITY_OF_SERVICE, *PSECURITY_QUALITY_OF_SERVICE, **PPSECURITY_QUALITY_OF_SERVICE ;

typedef struct _TOKEN_SOURCE { // 0x10 bytes
    uint8_t SourceName[8] ; // offset 0x0
    LUID SourceIdentifier ; // offset 0x8

} TOKEN_SOURCE, *PTOKEN_SOURCE, **PPTOKEN_SOURCE ;

typedef struct _TOKEN_CONTROL { // 0x28 bytes
    LUID TokenId ; // offset 0x0
    LUID AuthenticationId ; // offset 0x8
    LUID ModifiedId ; // offset 0x10
    TOKEN_SOURCE TokenSource ; // offset 0x18

} TOKEN_CONTROL, *PTOKEN_CONTROL, **PPTOKEN_CONTROL ;

typedef struct _SECURITY_CLIENT_CONTEXT { // 0x3c bytes
    SECURITY_QUALITY_OF_SERVICE SecurityQos ; // offset 0x0
    uint32_t ClientToken ; // offset 0xc
    uint8_t DirectlyAccessClientToken ; // offset 0x10
    uint8_t DirectAccessEffectiveOnly ; // offset 0x11
    uint8_t ServerIsRemote ; // offset 0x12
    TOKEN_CONTROL ClientTokenControl ; // offset 0x14

} SECURITY_CLIENT_CONTEXT, *PSECURITY_CLIENT_CONTEXT, **PPSECURITY_CLIENT_CONTEXT ;

typedef struct _LPCP_PORT_OBJECT { // 0xa4 bytes
    uint32_t ConnectionPort ; // offset 0x0
    uint32_t ConnectedPort ; // offset 0x4
    LPCP_PORT_QUEUE MsgQueue ; // offset 0x8
    CLIENT_ID Creator ; // offset 0x18
    uint32_t ClientSectionBase ; // offset 0x20
    uint32_t ServerSectionBase ; // offset 0x24
    uint32_t PortContext ; // offset 0x28
    uint32_t ClientThread ; // offset 0x2c
    SECURITY_QUALITY_OF_SERVICE SecurityQos ; // offset 0x30
    SECURITY_CLIENT_CONTEXT StaticSecurity ; // offset 0x3c
    LIST_ENTRY LpcReplyChainHead ; // offset 0x78
    LIST_ENTRY LpcDataInfoChainHead ; // offset 0x80
    union {
    uint32_t ServerProcess ; // offset 0x88
    uint32_t MappingProcess ; // offset 0x88
    };
    uint16_t MaxMessageLength ; // offset 0x8c
    uint16_t MaxConnectionInfoLength ; // offset 0x8e
    uint32_t Flags ; // offset 0x90
    KEVENT WaitEvent ; // offset 0x94

} LPCP_PORT_OBJECT, *PLPCP_PORT_OBJECT, **PPLPCP_PORT_OBJECT ;

typedef struct _SYSPTES_HEADER { // 0xc bytes
    LIST_ENTRY ListHead ; // offset 0x0
    uint32_t Count ; // offset 0x8

} SYSPTES_HEADER, *PSYSPTES_HEADER, **PPSYSPTES_HEADER ;

typedef union _SLIST_HEADER { // 0x8 bytes
    union {
    uint64_t Alignment ; // offset 0x0
    SINGLE_LIST_ENTRY Next ; // offset 0x0
    };
    uint16_t Depth ; // offset 0x4
    uint16_t Sequence ; // offset 0x6

} SLIST_HEADER, *PSLIST_HEADER, **PPSLIST_HEADER ;

typedef struct _HEAP_LOOKASIDE { // 0x30 bytes
    SLIST_HEADER ListHead ; // offset 0x0
    uint16_t Depth ; // offset 0x8
    uint16_t MaximumDepth ; // offset 0xa
    uint32_t TotalAllocates ; // offset 0xc
    uint32_t AllocateMisses ; // offset 0x10
    uint32_t TotalFrees ; // offset 0x14
    uint32_t FreeMisses ; // offset 0x18
    uint32_t LastTotalAllocates ; // offset 0x1c
    uint32_t LastAllocateMisses ; // offset 0x20
    uint32_t Counters[2] ; // offset 0x24

} HEAP_LOOKASIDE, *PHEAP_LOOKASIDE, **PPHEAP_LOOKASIDE ;

typedef struct _MMPFNENTRY { // 0x4 bytes
    union {
    uint32_t Modified : 1 ; // offset 0x0
    uint32_t ReadInProgress : 1 ; // offset 0x0
    uint32_t WriteInProgress : 1 ; // offset 0x0
    uint32_t PrototypePte : 1 ; // offset 0x0
    uint32_t PageColor : 3 ; // offset 0x0
    uint32_t ParityError : 1 ; // offset 0x0
    uint32_t PageLocation : 3 ; // offset 0x0
    uint32_t RemovalRequested : 1 ; // offset 0x0
    uint32_t CacheAttribute : 2 ; // offset 0x0
    uint32_t Rom : 1 ; // offset 0x0
    uint32_t LockCharged : 1 ; // offset 0x0
    uint32_t DontUse : 16 ; // offset 0x0
    };

} MMPFNENTRY, *PMMPFNENTRY, **PPMMPFNENTRY ;

typedef struct _HARDWARE_PTE { // 0x4 bytes
    union {
    uint32_t Valid : 1 ; // offset 0x0
    uint32_t Write : 1 ; // offset 0x0
    uint32_t Owner : 1 ; // offset 0x0
    uint32_t WriteThrough : 1 ; // offset 0x0
    uint32_t CacheDisable : 1 ; // offset 0x0
    uint32_t Accessed : 1 ; // offset 0x0
    uint32_t Dirty : 1 ; // offset 0x0
    uint32_t LargePage : 1 ; // offset 0x0
    uint32_t Global : 1 ; // offset 0x0
    uint32_t CopyOnWrite : 1 ; // offset 0x0
    uint32_t Prototype : 1 ; // offset 0x0
    uint32_t reserved : 1 ; // offset 0x0
    uint32_t PageFrameNumber : 20 ; // offset 0x0
    };

} HARDWARE_PTE, *PHARDWARE_PTE, **PPHARDWARE_PTE ;

typedef struct _MMPTE_PROTOTYPE { // 0x4 bytes
    union {
    uint32_t Valid : 1 ; // offset 0x0
    uint32_t ProtoAddressLow : 7 ; // offset 0x0
    uint32_t ReadOnly : 1 ; // offset 0x0
    uint32_t WhichPool : 1 ; // offset 0x0
    uint32_t Prototype : 1 ; // offset 0x0
    uint32_t ProtoAddressHigh : 21 ; // offset 0x0
    };

} MMPTE_PROTOTYPE, *PMMPTE_PROTOTYPE, **PPMMPTE_PROTOTYPE ;

typedef struct _MMPTE_HARDWARE { // 0x4 bytes
    union {
    uint32_t Valid : 1 ; // offset 0x0
    uint32_t Write : 1 ; // offset 0x0
    uint32_t Owner : 1 ; // offset 0x0
    uint32_t WriteThrough : 1 ; // offset 0x0
    uint32_t CacheDisable : 1 ; // offset 0x0
    uint32_t Accessed : 1 ; // offset 0x0
    uint32_t Dirty : 1 ; // offset 0x0
    uint32_t LargePage : 1 ; // offset 0x0
    uint32_t Global : 1 ; // offset 0x0
    uint32_t CopyOnWrite : 1 ; // offset 0x0
    uint32_t Prototype : 1 ; // offset 0x0
    uint32_t reserved : 1 ; // offset 0x0
    uint32_t PageFrameNumber : 20 ; // offset 0x0
    };

} MMPTE_HARDWARE, *PMMPTE_HARDWARE, **PPMMPTE_HARDWARE ;

typedef struct _MMPTE_SOFTWARE { // 0x4 bytes
    union {
    uint32_t Valid : 1 ; // offset 0x0
    uint32_t PageFileLow : 4 ; // offset 0x0
    uint32_t Protection : 5 ; // offset 0x0
    uint32_t Prototype : 1 ; // offset 0x0
    uint32_t Transition : 1 ; // offset 0x0
    uint32_t PageFileHigh : 20 ; // offset 0x0
    };

} MMPTE_SOFTWARE, *PMMPTE_SOFTWARE, **PPMMPTE_SOFTWARE ;

typedef struct _MMPTE_SUBSECTION { // 0x4 bytes
    union {
    uint32_t Valid : 1 ; // offset 0x0
    uint32_t SubsectionAddressLow : 4 ; // offset 0x0
    uint32_t Protection : 5 ; // offset 0x0
    uint32_t Prototype : 1 ; // offset 0x0
    uint32_t SubsectionAddressHigh : 20 ; // offset 0x0
    uint32_t WhichPool : 1 ; // offset 0x0
    };

} MMPTE_SUBSECTION, *PMMPTE_SUBSECTION, **PPMMPTE_SUBSECTION ;

typedef struct _MMPTE_TRANSITION { // 0x4 bytes
    union {
    uint32_t Valid : 1 ; // offset 0x0
    uint32_t Write : 1 ; // offset 0x0
    uint32_t Owner : 1 ; // offset 0x0
    uint32_t WriteThrough : 1 ; // offset 0x0
    uint32_t CacheDisable : 1 ; // offset 0x0
    uint32_t Protection : 5 ; // offset 0x0
    uint32_t Prototype : 1 ; // offset 0x0
    uint32_t Transition : 1 ; // offset 0x0
    uint32_t PageFrameNumber : 20 ; // offset 0x0
    };

} MMPTE_TRANSITION, *PMMPTE_TRANSITION, **PPMMPTE_TRANSITION ;

typedef struct _MMPTE_LIST { // 0x4 bytes
    union {
    uint32_t Valid : 1 ; // offset 0x0
    uint32_t OneEntry : 1 ; // offset 0x0
    uint32_t filler0 : 8 ; // offset 0x0
    uint32_t Prototype : 1 ; // offset 0x0
    uint32_t filler1 : 1 ; // offset 0x0
    uint32_t NextEntry : 20 ; // offset 0x0
    };

} MMPTE_LIST, *PMMPTE_LIST, **PPMMPTE_LIST ;

typedef struct _MMPTE { // 0x4 bytes
    union {
        union {
        uint32_t Long ; // offset 0x0
        MMPTE_HARDWARE Hard ; // offset 0x0
        HARDWARE_PTE Flush ; // offset 0x0
        MMPTE_PROTOTYPE Proto ; // offset 0x0
        MMPTE_SOFTWARE Soft ; // offset 0x0
        MMPTE_TRANSITION Trans ; // offset 0x0
        MMPTE_SUBSECTION Subsect ; // offset 0x0
        MMPTE_LIST List ; // offset 0x0
        };
    } u ; // offset 0x0

} MMPTE, *PMMPTE, **PPMMPTE ;

typedef struct _MMPFN { // 0x18 bytes
    union {
        union {
        uint32_t Flink ; // offset 0x0
        uint32_t WsIndex ; // offset 0x0
        uint32_t Event ; // offset 0x0
        int32_t ReadStatus ; // offset 0x0
        SINGLE_LIST_ENTRY NextStackPfn ; // offset 0x0
        };
    } u1 ; // offset 0x0
    uint32_t PteAddress ; // offset 0x4
    union {
        union {
        uint32_t Blink ; // offset 0x0
        uint32_t ShareCount ; // offset 0x0
        };
    } u2 ; // offset 0x8
    union {
        union {
        MMPFNENTRY e1 ; // offset 0x0
        struct {
                    uint16_t ShortFlags ; // offset 0x0
                    uint16_t ReferenceCount ; // offset 0x2
                } e2 ; // offset 0x0
        };
    } u3 ; // offset 0xc
    MMPTE OriginalPte ; // offset 0x10
    union {
        union {
        uint32_t EntireFrame ; // offset 0x0
        uint32_t PteFrame : 26 ; // offset 0x0
        uint32_t InPageError : 1 ; // offset 0x0
        uint32_t VerifierAllocation : 1 ; // offset 0x0
        uint32_t AweAllocation : 1 ; // offset 0x0
        uint32_t LockCharged : 1 ; // offset 0x0
        uint32_t KernelStack : 1 ; // offset 0x0
        uint32_t Reserved : 1 ; // offset 0x0
        };
    } u4 ; // offset 0x14

} MMPFN, *PMMPFN, **PPMMPFN ;

typedef struct _POOL_DESCRIPTOR { // 0x1028 bytes
    POOL_TYPE PoolType ; // offset 0x0
    uint32_t PoolIndex ; // offset 0x4
    uint32_t RunningAllocs ; // offset 0x8
    uint32_t RunningDeAllocs ; // offset 0xc
    uint32_t TotalPages ; // offset 0x10
    uint32_t TotalBigPages ; // offset 0x14
    uint32_t Threshold ; // offset 0x18
    uint32_t LockAddress ; // offset 0x1c
    uint32_t PendingFrees ; // offset 0x20
    int32_t PendingFreeDepth ; // offset 0x24
    LIST_ENTRY ListHeads[512] ; // offset 0x28

} POOL_DESCRIPTOR, *PPOOL_DESCRIPTOR, **PPPOOL_DESCRIPTOR ;

typedef struct _MMSUPPORT_FLAGS { // 0x4 bytes
    union {
    uint32_t SessionSpace : 1 ; // offset 0x0
    uint32_t BeingTrimmed : 1 ; // offset 0x0
    uint32_t SessionLeader : 1 ; // offset 0x0
    uint32_t TrimHard : 1 ; // offset 0x0
    uint32_t WorkingSetHard : 1 ; // offset 0x0
    uint32_t AddressSpaceBeingDeleted : 1 ; // offset 0x0
    uint32_t Available : 10 ; // offset 0x0
    uint32_t AllowWorkingSetAdjustment : 8 ; // offset 0x0
    uint32_t MemoryPriority : 8 ; // offset 0x0
    };

} MMSUPPORT_FLAGS, *PMMSUPPORT_FLAGS, **PPMMSUPPORT_FLAGS ;

typedef struct _MMSUPPORT { // 0x40 bytes
    LARGE_INTEGER LastTrimTime ; // offset 0x0
    MMSUPPORT_FLAGS Flags ; // offset 0x8
    uint32_t PageFaultCount ; // offset 0xc
    uint32_t PeakWorkingSetSize ; // offset 0x10
    uint32_t WorkingSetSize ; // offset 0x14
    uint32_t MinimumWorkingSetSize ; // offset 0x18
    uint32_t MaximumWorkingSetSize ; // offset 0x1c
    uint32_t VmWorkingSetList ; // offset 0x20
    LIST_ENTRY WorkingSetExpansionLinks ; // offset 0x24
    uint32_t Claim ; // offset 0x2c
    uint32_t NextEstimationSlot ; // offset 0x30
    uint32_t NextAgingSlot ; // offset 0x34
    uint32_t EstimatedAvailable ; // offset 0x38
    uint32_t GrowthSinceLastEstimate ; // offset 0x3c

} MMSUPPORT, *PMMSUPPORT, **PPMMSUPPORT ;

typedef struct _MM_SESSION_SPACE_FLAGS { // 0x4 bytes
    union {
    uint32_t Initialized : 1 ; // offset 0x0
    uint32_t Filler0 : 3 ; // offset 0x0
    uint32_t HasWsLock : 1 ; // offset 0x0
    uint32_t DeletePending : 1 ; // offset 0x0
    uint32_t Filler : 26 ; // offset 0x0
    };

} MM_SESSION_SPACE_FLAGS, *PMM_SESSION_SPACE_FLAGS, **PPMM_SESSION_SPACE_FLAGS ;

typedef struct _DRIVER_OBJECT { // 0xa8 bytes
    int16_t Type ; // offset 0x0
    int16_t Size ; // offset 0x2
    uint32_t DeviceObject ; // offset 0x4
    uint32_t Flags ; // offset 0x8
    uint32_t DriverStart ; // offset 0xc
    uint32_t DriverSize ; // offset 0x10
    uint32_t DriverSection ; // offset 0x14
    uint32_t DriverExtension ; // offset 0x18
    UNICODE_STRING DriverName ; // offset 0x1c
    uint32_t HardwareDatabase ; // offset 0x24
    uint32_t FastIoDispatch ; // offset 0x28
    uint32_t DriverInit ; // offset 0x2c
    uint32_t DriverStartIo ; // offset 0x30
    uint32_t DriverUnload ; // offset 0x34
    uint32_t MajorFunction[28] ; // offset 0x38

} DRIVER_OBJECT, *PDRIVER_OBJECT, **PPDRIVER_OBJECT ;

typedef struct _FAST_MUTEX { // 0x20 bytes
    int32_t Count ; // offset 0x0
    uint32_t Owner ; // offset 0x4
    uint32_t Contention ; // offset 0x8
    KEVENT Event ; // offset 0xc
    uint32_t OldIrql ; // offset 0x1c

} FAST_MUTEX, *PFAST_MUTEX, **PPFAST_MUTEX ;

typedef struct _MMSESSION { // 0x3c bytes
    FAST_MUTEX SystemSpaceViewLock ; // offset 0x0
    uint32_t SystemSpaceViewLockPointer ; // offset 0x20
    uint32_t SystemSpaceViewStart ; // offset 0x24
    uint32_t SystemSpaceViewTable ; // offset 0x28
    uint32_t SystemSpaceHashSize ; // offset 0x2c
    uint32_t SystemSpaceHashEntries ; // offset 0x30
    uint32_t SystemSpaceHashKey ; // offset 0x34
    uint32_t SystemSpaceBitMap ; // offset 0x38

} MMSESSION, *PMMSESSION, **PPMMSESSION ;

typedef struct _MM_PAGED_POOL_INFO { // 0x24 bytes
    uint32_t PagedPoolAllocationMap ; // offset 0x0
    uint32_t EndOfPagedPoolBitmap ; // offset 0x4
    uint32_t PagedPoolLargeSessionAllocationMap ; // offset 0x8
    uint32_t FirstPteForPagedPool ; // offset 0xc
    uint32_t LastPteForPagedPool ; // offset 0x10
    uint32_t NextPdeForPagedPoolExpansion ; // offset 0x14
    uint32_t PagedPoolHint ; // offset 0x18
    uint32_t PagedPoolCommit ; // offset 0x1c
    uint32_t AllocatedPagedPool ; // offset 0x20

} MM_PAGED_POOL_INFO, *PMM_PAGED_POOL_INFO, **PPMM_PAGED_POOL_INFO ;

typedef struct _OWNER_ENTRY { // 0x8 bytes
    uint32_t OwnerThread ; // offset 0x0
    union {
    int32_t OwnerCount ; // offset 0x4
    uint32_t TableSize ; // offset 0x4
    };

} OWNER_ENTRY, *POWNER_ENTRY, **PPOWNER_ENTRY ;

typedef struct _ERESOURCE { // 0x38 bytes
    LIST_ENTRY SystemResourcesList ; // offset 0x0
    uint32_t OwnerTable ; // offset 0x8
    int16_t ActiveCount ; // offset 0xc
    uint16_t Flag ; // offset 0xe
    uint32_t SharedWaiters ; // offset 0x10
    uint32_t ExclusiveWaiters ; // offset 0x14
    OWNER_ENTRY OwnerThreads[2] ; // offset 0x18
    uint32_t ContentionCount ; // offset 0x28
    uint16_t NumberOfSharedWaiters ; // offset 0x2c
    uint16_t NumberOfExclusiveWaiters ; // offset 0x2e
    union {
    uint32_t Address ; // offset 0x30
    uint32_t CreatorBackTraceIndex ; // offset 0x30
    };
    uint32_t SpinLock ; // offset 0x34

} ERESOURCE, *PERESOURCE, **PPERESOURCE ;

typedef struct _MM_SESSION_SPACE { // 0x1278 bytes
    uint32_t ReferenceCount ; // offset 0x0
    union {
        union {
        uint32_t LongFlags ; // offset 0x0
        MM_SESSION_SPACE_FLAGS Flags ; // offset 0x0
        };
    } u ; // offset 0x4
    uint32_t SessionId ; // offset 0x8
    uint32_t SessionPageDirectoryIndex ; // offset 0xc
    uint32_t GlobalVirtualAddress ; // offset 0x10
    LIST_ENTRY ProcessList ; // offset 0x14
    uint32_t NonPagedPoolBytes ; // offset 0x1c
    uint32_t PagedPoolBytes ; // offset 0x20
    uint32_t NonPagedPoolAllocations ; // offset 0x24
    uint32_t PagedPoolAllocations ; // offset 0x28
    uint32_t NonPagablePages ; // offset 0x2c
    uint32_t CommittedPages ; // offset 0x30
    LARGE_INTEGER LastProcessSwappedOutTime ; // offset 0x38
    uint32_t PageTables ; // offset 0x40
    FAST_MUTEX PagedPoolMutex ; // offset 0x44
    uint32_t PagedPoolStart ; // offset 0x64
    uint32_t PagedPoolEnd ; // offset 0x68
    uint32_t PagedPoolBasePde ; // offset 0x6c
    MM_PAGED_POOL_INFO PagedPoolInfo ; // offset 0x70
    uint32_t Color ; // offset 0x94
    uint32_t ProcessOutSwapCount ; // offset 0x98
    LIST_ENTRY ImageList ; // offset 0x9c
    uint32_t GlobalPteEntry ; // offset 0xa4
    uint32_t CopyOnWriteCount ; // offset 0xa8
    uint32_t SessionPoolAllocationFailures[4] ; // offset 0xac
    uint32_t AttachCount ; // offset 0xbc
    KEVENT AttachEvent ; // offset 0xc0
    uint32_t LastProcess ; // offset 0xd0
    MMSUPPORT Vm ; // offset 0xd8
    uint32_t Wsle ; // offset 0x118
    ERESOURCE WsLock ; // offset 0x11c
    LIST_ENTRY WsListEntry ; // offset 0x154
    MMSESSION Session ; // offset 0x15c
    DRIVER_OBJECT Win32KDriverObject ; // offset 0x198
    uint32_t WorkingSetLockOwner ; // offset 0x240
    POOL_DESCRIPTOR PagedPool ; // offset 0x244
    int32_t ProcessReferenceToSession ; // offset 0x126c
    uint32_t LocaleId ; // offset 0x1270

} MM_SESSION_SPACE, *PMM_SESSION_SPACE, **PPMM_SESSION_SPACE ;

typedef struct _MMBANKED_SECTION { // 0x20 bytes
    uint32_t BasePhysicalPage ; // offset 0x0
    uint32_t BasedPte ; // offset 0x4
    uint32_t BankSize ; // offset 0x8
    uint32_t BankShift ; // offset 0xc
    uint32_t BankedRoutine ; // offset 0x10
    uint32_t Context ; // offset 0x14
    uint32_t CurrentMappedPte ; // offset 0x18
    MMPTE BankTemplate[1] ; // offset 0x1c

} MMBANKED_SECTION, *PMMBANKED_SECTION, **PPMMBANKED_SECTION ;

typedef struct _SUPPORTED_RANGE { // 0x20 bytes
    uint32_t Next ; // offset 0x0
    uint32_t SystemAddressSpace ; // offset 0x4
    int64_t SystemBase ; // offset 0x8
    int64_t Base ; // offset 0x10
    int64_t Limit ; // offset 0x18

} SUPPORTED_RANGE, *PSUPPORTED_RANGE, **PPSUPPORTED_RANGE ;

typedef struct _SUPPORTED_RANGES { // 0xa0 bytes
    uint16_t Version ; // offset 0x0
    uint8_t Sorted ; // offset 0x2
    uint8_t Reserved ; // offset 0x3
    uint32_t NoIO ; // offset 0x4
    SUPPORTED_RANGE IO ; // offset 0x8
    uint32_t NoMemory ; // offset 0x28
    SUPPORTED_RANGE Memory ; // offset 0x30
    uint32_t NoPrefetchMemory ; // offset 0x50
    SUPPORTED_RANGE PrefetchMemory ; // offset 0x58
    uint32_t NoDma ; // offset 0x78
    SUPPORTED_RANGE Dma ; // offset 0x80

} SUPPORTED_RANGES, *PSUPPORTED_RANGES, **PPSUPPORTED_RANGES ;

typedef struct _GENERAL_LOOKASIDE { // 0x80 bytes
    SLIST_HEADER ListHead ; // offset 0x0
    uint16_t Depth ; // offset 0x8
    uint16_t MaximumDepth ; // offset 0xa
    uint32_t TotalAllocates ; // offset 0xc
    union {
    uint32_t AllocateMisses ; // offset 0x10
    uint32_t AllocateHits ; // offset 0x10
    };
    uint32_t TotalFrees ; // offset 0x14
    union {
    uint32_t FreeMisses ; // offset 0x18
    uint32_t FreeHits ; // offset 0x18
    };
    POOL_TYPE Type ; // offset 0x1c
    uint32_t Tag ; // offset 0x20
    uint32_t Size ; // offset 0x24
    uint32_t Allocate ; // offset 0x28
    uint32_t Free ; // offset 0x2c
    LIST_ENTRY ListEntry ; // offset 0x30
    uint32_t LastTotalAllocates ; // offset 0x38
    union {
    uint32_t LastAllocateMisses ; // offset 0x3c
    uint32_t LastAllocateHits ; // offset 0x3c
    };
    uint32_t Future[2] ; // offset 0x40

} GENERAL_LOOKASIDE, *PGENERAL_LOOKASIDE, **PPGENERAL_LOOKASIDE ;

typedef struct _NPAGED_LOOKASIDE_LIST { // 0x100 bytes
    GENERAL_LOOKASIDE L ; // offset 0x0
    uint32_t Lock__ObsoleteButDoNotDelete ; // offset 0x80

} NPAGED_LOOKASIDE_LIST, *PNPAGED_LOOKASIDE_LIST, **PPNPAGED_LOOKASIDE_LIST ;

typedef struct _BUS_HANDLER { // 0x6c bytes
    uint32_t Version ; // offset 0x0
    INTERFACE_TYPE InterfaceType ; // offset 0x4
    BUS_DATA_TYPE ConfigurationType ; // offset 0x8
    uint32_t BusNumber ; // offset 0xc
    uint32_t DeviceObject ; // offset 0x10
    uint32_t ParentHandler ; // offset 0x14
    uint32_t BusData ; // offset 0x18
    uint32_t DeviceControlExtensionSize ; // offset 0x1c
    uint32_t BusAddresses ; // offset 0x20
    uint32_t Reserved[4] ; // offset 0x24
    uint32_t GetBusData ; // offset 0x34
    uint32_t SetBusData ; // offset 0x38
    uint32_t AdjustResourceList ; // offset 0x3c
    uint32_t AssignSlotResources ; // offset 0x40
    uint32_t GetInterruptVector ; // offset 0x44
    uint32_t TranslateBusAddress ; // offset 0x48
    uint32_t Spare1 ; // offset 0x4c
    uint32_t Spare2 ; // offset 0x50
    uint32_t Spare3 ; // offset 0x54
    uint32_t Spare4 ; // offset 0x58
    uint32_t Spare5 ; // offset 0x5c
    uint32_t Spare6 ; // offset 0x60
    uint32_t Spare7 ; // offset 0x64
    uint32_t Spare8 ; // offset 0x68

} BUS_HANDLER, *PBUS_HANDLER, **PPBUS_HANDLER ;

typedef struct _POP_ACTION_TRIGGER { // 0xc bytes
    POP_POLICY_DEVICE_TYPE Type ; // offset 0x0
    uint8_t Flags ; // offset 0x4
    uint8_t Spare[3] ; // offset 0x5
    union {
    struct {
            uint32_t Level ; // offset 0x0
        } Battery ; // offset 0x8
    uint32_t Wait ; // offset 0x8
    };

} POP_ACTION_TRIGGER, *PPOP_ACTION_TRIGGER, **PPPOP_ACTION_TRIGGER ;

typedef struct _THERMAL_INFORMATION { // 0x4c bytes
    uint32_t ThermalStamp ; // offset 0x0
    uint32_t ThermalConstant1 ; // offset 0x4
    uint32_t ThermalConstant2 ; // offset 0x8
    uint32_t Processors ; // offset 0xc
    uint32_t SamplingPeriod ; // offset 0x10
    uint32_t CurrentTemperature ; // offset 0x14
    uint32_t PassiveTripPoint ; // offset 0x18
    uint32_t CriticalTripPoint ; // offset 0x1c
    uint8_t ActiveTripPointCount ; // offset 0x20
    uint32_t ActiveTripPoint[10] ; // offset 0x24

} THERMAL_INFORMATION, *PTHERMAL_INFORMATION, **PPTHERMAL_INFORMATION ;

typedef struct _POP_THERMAL_ZONE { // 0xd0 bytes
    LIST_ENTRY Link ; // offset 0x0
    uint8_t State ; // offset 0x8
    uint8_t Flags ; // offset 0x9
    uint8_t Mode ; // offset 0xa
    uint8_t PendingMode ; // offset 0xb
    uint8_t ActivePoint ; // offset 0xc
    uint8_t PendingActivePoint ; // offset 0xd
    int32_t Throttle ; // offset 0x10
    uint64_t LastTime ; // offset 0x18
    uint32_t SampleRate ; // offset 0x20
    uint32_t LastTemp ; // offset 0x24
    KTIMER PassiveTimer ; // offset 0x28
    KDPC PassiveDpc ; // offset 0x50
    POP_ACTION_TRIGGER OverThrottled ; // offset 0x70
    uint32_t Irp ; // offset 0x7c
    THERMAL_INFORMATION Info ; // offset 0x80

} POP_THERMAL_ZONE, *PPOP_THERMAL_ZONE, **PPPOP_THERMAL_ZONE ;

typedef struct _MMVAD_SHORT { // 0x18 bytes
    uint32_t StartingVpn ; // offset 0x0
    uint32_t EndingVpn ; // offset 0x4
    uint32_t Parent ; // offset 0x8
    uint32_t LeftChild ; // offset 0xc
    uint32_t RightChild ; // offset 0x10
    union {
        union {
        uint32_t LongFlags ; // offset 0x0
        MMVAD_FLAGS VadFlags ; // offset 0x0
        };
    } u ; // offset 0x14

} MMVAD_SHORT, *PMMVAD_SHORT, **PPMMVAD_SHORT ;

typedef struct _TERMINATION_PORT { // 0x8 bytes
    uint32_t Next ; // offset 0x0
    uint32_t Port ; // offset 0x4

} TERMINATION_PORT, *PTERMINATION_PORT, **PPTERMINATION_PORT ;

typedef struct _GENERIC_MAPPING { // 0x10 bytes
    uint32_t GenericRead ; // offset 0x0
    uint32_t GenericWrite ; // offset 0x4
    uint32_t GenericExecute ; // offset 0x8
    uint32_t GenericAll ; // offset 0xc

} GENERIC_MAPPING, *PGENERIC_MAPPING, **PPGENERIC_MAPPING ;

typedef struct _OBJECT_TYPE_INITIALIZER { // 0x4c bytes
    uint16_t Length ; // offset 0x0
    uint8_t UseDefaultObject ; // offset 0x2
    uint8_t CaseInsensitive ; // offset 0x3
    uint32_t InvalidAttributes ; // offset 0x4
    GENERIC_MAPPING GenericMapping ; // offset 0x8
    uint32_t ValidAccessMask ; // offset 0x18
    uint8_t SecurityRequired ; // offset 0x1c
    uint8_t MaintainHandleCount ; // offset 0x1d
    uint8_t MaintainTypeList ; // offset 0x1e
    POOL_TYPE PoolType ; // offset 0x20
    uint32_t DefaultPagedPoolCharge ; // offset 0x24
    uint32_t DefaultNonPagedPoolCharge ; // offset 0x28
    uint32_t DumpProcedure ; // offset 0x2c
    uint32_t OpenProcedure ; // offset 0x30
    uint32_t CloseProcedure ; // offset 0x34
    uint32_t DeleteProcedure ; // offset 0x38
    uint32_t ParseProcedure ; // offset 0x3c
    uint32_t SecurityProcedure ; // offset 0x40
    uint32_t QueryNameProcedure ; // offset 0x44
    uint32_t OkayToCloseProcedure ; // offset 0x48

} OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER, **PPOBJECT_TYPE_INITIALIZER ;

typedef struct _OBJECT_TYPE { // 0x190 bytes
    ERESOURCE Mutex ; // offset 0x0
    LIST_ENTRY TypeList ; // offset 0x38
    UNICODE_STRING Name ; // offset 0x40
    uint32_t DefaultObject ; // offset 0x48
    uint32_t Index ; // offset 0x4c
    uint32_t TotalNumberOfObjects ; // offset 0x50
    uint32_t TotalNumberOfHandles ; // offset 0x54
    uint32_t HighWaterNumberOfObjects ; // offset 0x58
    uint32_t HighWaterNumberOfHandles ; // offset 0x5c
    OBJECT_TYPE_INITIALIZER TypeInfo ; // offset 0x60
    uint32_t Key ; // offset 0xac
    ERESOURCE ObjectLocks[4] ; // offset 0xb0

} OBJECT_TYPE, *POBJECT_TYPE, **PPOBJECT_TYPE ;

typedef struct _HANDLE_TABLE_ENTRY_INFO { // 0x4 bytes
    uint32_t AuditMask ; // offset 0x0

} HANDLE_TABLE_ENTRY_INFO, *PHANDLE_TABLE_ENTRY_INFO, **PPHANDLE_TABLE_ENTRY_INFO ;

typedef struct _PO_HIBER_PERF { // 0x48 bytes
    uint64_t IoTicks ; // offset 0x0
    uint64_t InitTicks ; // offset 0x8
    uint64_t CopyTicks ; // offset 0x10
    uint64_t StartCount ; // offset 0x18
    uint32_t ElapsedTime ; // offset 0x20
    uint32_t IoTime ; // offset 0x24
    uint32_t CopyTime ; // offset 0x28
    uint32_t InitTime ; // offset 0x2c
    uint32_t PagesWritten ; // offset 0x30
    uint32_t PagesProcessed ; // offset 0x34
    uint32_t BytesCopied ; // offset 0x38
    uint32_t DumpCount ; // offset 0x3c
    uint32_t FileRuns ; // offset 0x40

} PO_HIBER_PERF, *PPO_HIBER_PERF, **PPPO_HIBER_PERF ;

typedef struct _PO_MEMORY_IMAGE { // 0xa8 bytes
    uint32_t Signature ; // offset 0x0
    uint32_t Version ; // offset 0x4
    uint32_t CheckSum ; // offset 0x8
    uint32_t LengthSelf ; // offset 0xc
    uint32_t PageSelf ; // offset 0x10
    uint32_t PageSize ; // offset 0x14
    uint32_t ImageType ; // offset 0x18
    LARGE_INTEGER SystemTime ; // offset 0x20
    uint64_t InterruptTime ; // offset 0x28
    uint32_t FeatureFlags ; // offset 0x30
    uint8_t HiberFlags ; // offset 0x34
    uint8_t spare[3] ; // offset 0x35
    uint32_t NoHiberPtes ; // offset 0x38
    uint32_t HiberVa ; // offset 0x3c
    LARGE_INTEGER HiberPte ; // offset 0x40
    uint32_t NoFreePages ; // offset 0x48
    uint32_t FreeMapCheck ; // offset 0x4c
    uint32_t WakeCheck ; // offset 0x50
    uint32_t TotalPages ; // offset 0x54
    uint32_t FirstTablePage ; // offset 0x58
    uint32_t LastFilePage ; // offset 0x5c
    PO_HIBER_PERF PerfInfo ; // offset 0x60

} PO_MEMORY_IMAGE, *PPO_MEMORY_IMAGE, **PPPO_MEMORY_IMAGE ;

typedef struct _PO_DEVICE_NOTIFY { // 0x28 bytes
    LIST_ENTRY Link ; // offset 0x0
    uint32_t TargetDevice ; // offset 0x8
    uint8_t WakeNeeded ; // offset 0xc
    uint8_t OrderLevel ; // offset 0xd
    uint32_t DeviceObject ; // offset 0x10
    uint32_t Node ; // offset 0x14
    uint32_t DeviceName ; // offset 0x18
    uint32_t DriverName ; // offset 0x1c
    uint32_t ChildCount ; // offset 0x20
    uint32_t ActiveChild ; // offset 0x24

} PO_DEVICE_NOTIFY, *PPO_DEVICE_NOTIFY, **PPPO_DEVICE_NOTIFY ;

typedef struct _MAILSLOT_CREATE_PARAMETERS { // 0x18 bytes
    uint32_t MailslotQuota ; // offset 0x0
    uint32_t MaximumMessageSize ; // offset 0x4
    LARGE_INTEGER ReadTimeout ; // offset 0x8
    uint8_t TimeoutSpecified ; // offset 0x10

} MAILSLOT_CREATE_PARAMETERS, *PMAILSLOT_CREATE_PARAMETERS, **PPMAILSLOT_CREATE_PARAMETERS ;

typedef struct _KSYSTEM_TIME { // 0xc bytes
    uint32_t LowPart ; // offset 0x0
    int32_t High1Time ; // offset 0x4
    int32_t High2Time ; // offset 0x8

} KSYSTEM_TIME, *PKSYSTEM_TIME, **PPKSYSTEM_TIME ;

typedef struct _KUSER_SHARED_DATA { // 0x338 bytes
    volatile uint32_t TickCountLow ; // offset 0x0
    uint32_t TickCountMultiplier ; // offset 0x4
    volatile KSYSTEM_TIME InterruptTime ; // offset 0x8
    volatile KSYSTEM_TIME SystemTime ; // offset 0x14
    volatile KSYSTEM_TIME TimeZoneBias ; // offset 0x20
    uint16_t ImageNumberLow ; // offset 0x2c
    uint16_t ImageNumberHigh ; // offset 0x2e
    uint16_t NtSystemRoot[260] ; // offset 0x30
    uint32_t MaxStackTraceDepth ; // offset 0x238
    uint32_t CryptoExponent ; // offset 0x23c
    uint32_t TimeZoneId ; // offset 0x240
    uint32_t Reserved2[8] ; // offset 0x244
    NT_PRODUCT_TYPE NtProductType ; // offset 0x264
    uint8_t ProductTypeIsValid ; // offset 0x268
    uint32_t NtMajorVersion ; // offset 0x26c
    uint32_t NtMinorVersion ; // offset 0x270
    uint8_t ProcessorFeatures[64] ; // offset 0x274
    uint32_t Reserved1 ; // offset 0x2b4
    uint32_t Reserved3 ; // offset 0x2b8
    volatile uint32_t TimeSlip ; // offset 0x2bc
    ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture ; // offset 0x2c0
    LARGE_INTEGER SystemExpirationDate ; // offset 0x2c8
    uint32_t SuiteMask ; // offset 0x2d0
    uint8_t KdDebuggerEnabled ; // offset 0x2d4
    uint8_t NXSupportPolicy ; // offset 0x2d5
    volatile uint32_t ActiveConsoleId ; // offset 0x2d8
    volatile uint32_t DismountCount ; // offset 0x2dc
    uint32_t ComPlusPackage ; // offset 0x2e0
    uint32_t LastSystemRITEventTickCount ; // offset 0x2e4
    uint32_t NumberOfPhysicalPages ; // offset 0x2e8
    uint8_t SafeBootMode ; // offset 0x2ec
    uint32_t TraceLogging ; // offset 0x2f0
    uint64_t TestRetInstruction ; // offset 0x2f8
    uint32_t SystemCall ; // offset 0x300
    uint32_t SystemCallReturn ; // offset 0x304
    uint64_t SystemCallPad[3] ; // offset 0x308
    union {
    volatile KSYSTEM_TIME TickCount ; // offset 0x320
    volatile uint64_t TickCountQuad ; // offset 0x320
    };
    uint32_t Cookie ; // offset 0x330

} KUSER_SHARED_DATA, *PKUSER_SHARED_DATA, **PPKUSER_SHARED_DATA ;

typedef struct _PS_JOB_TOKEN_FILTER { // 0x24 bytes
    uint32_t CapturedSidCount ; // offset 0x0
    uint32_t CapturedSids ; // offset 0x4
    uint32_t CapturedSidsLength ; // offset 0x8
    uint32_t CapturedGroupCount ; // offset 0xc
    uint32_t CapturedGroups ; // offset 0x10
    uint32_t CapturedGroupsLength ; // offset 0x14
    uint32_t CapturedPrivilegeCount ; // offset 0x18
    uint32_t CapturedPrivileges ; // offset 0x1c
    uint32_t CapturedPrivilegesLength ; // offset 0x20

} PS_JOB_TOKEN_FILTER, *PPS_JOB_TOKEN_FILTER, **PPPS_JOB_TOKEN_FILTER ;

typedef struct _HEAP_ENTRY_EXTRA { // 0x8 bytes
    uint16_t AllocatorBackTraceIndex ; // offset 0x0
    uint16_t TagIndex ; // offset 0x2
    uint32_t Settable ; // offset 0x4
    uint64_t ZeroInit ; // offset 0x0

} HEAP_ENTRY_EXTRA, *PHEAP_ENTRY_EXTRA, **PPHEAP_ENTRY_EXTRA ;

typedef struct _HEAP_VIRTUAL_ALLOC_ENTRY { // 0x20 bytes
    LIST_ENTRY Entry ; // offset 0x0
    HEAP_ENTRY_EXTRA ExtraStuff ; // offset 0x8
    uint32_t CommitSize ; // offset 0x10
    uint32_t ReserveSize ; // offset 0x14
    HEAP_ENTRY BusyBlock ; // offset 0x18

} HEAP_VIRTUAL_ALLOC_ENTRY, *PHEAP_VIRTUAL_ALLOC_ENTRY, **PPHEAP_VIRTUAL_ALLOC_ENTRY ;

typedef struct _CMP_OFFSET_ARRAY { // 0xc bytes
    uint32_t FileOffset ; // offset 0x0
    uint32_t DataBuffer ; // offset 0x4
    uint32_t DataLength ; // offset 0x8

} CMP_OFFSET_ARRAY, *PCMP_OFFSET_ARRAY, **PPCMP_OFFSET_ARRAY ;

typedef struct _FILE_OBJECT { // 0x70 bytes
    int16_t Type ; // offset 0x0
    int16_t Size ; // offset 0x2
    uint32_t DeviceObject ; // offset 0x4
    uint32_t Vpb ; // offset 0x8
    uint32_t FsContext ; // offset 0xc
    uint32_t FsContext2 ; // offset 0x10
    uint32_t SectionObjectPointer ; // offset 0x14
    uint32_t PrivateCacheMap ; // offset 0x18
    int32_t FinalStatus ; // offset 0x1c
    uint32_t RelatedFileObject ; // offset 0x20
    uint8_t LockOperation ; // offset 0x24
    uint8_t DeletePending ; // offset 0x25
    uint8_t ReadAccess ; // offset 0x26
    uint8_t WriteAccess ; // offset 0x27
    uint8_t DeleteAccess ; // offset 0x28
    uint8_t SharedRead ; // offset 0x29
    uint8_t SharedWrite ; // offset 0x2a
    uint8_t SharedDelete ; // offset 0x2b
    uint32_t Flags ; // offset 0x2c
    UNICODE_STRING FileName ; // offset 0x30
    LARGE_INTEGER CurrentByteOffset ; // offset 0x38
    uint32_t Waiters ; // offset 0x40
    uint32_t Busy ; // offset 0x44
    uint32_t LastLock ; // offset 0x48
    KEVENT Lock ; // offset 0x4c
    KEVENT Event ; // offset 0x5c
    uint32_t CompletionContext ; // offset 0x6c

} FILE_OBJECT, *PFILE_OBJECT, **PPFILE_OBJECT ;

typedef struct _IMAGE_DOS_HEADER { // 0x40 bytes
    uint16_t e_magic ; // offset 0x0
    uint16_t e_cblp ; // offset 0x2
    uint16_t e_cp ; // offset 0x4
    uint16_t e_crlc ; // offset 0x6
    uint16_t e_cparhdr ; // offset 0x8
    uint16_t e_minalloc ; // offset 0xa
    uint16_t e_maxalloc ; // offset 0xc
    uint16_t e_ss ; // offset 0xe
    uint16_t e_sp ; // offset 0x10
    uint16_t e_csum ; // offset 0x12
    uint16_t e_ip ; // offset 0x14
    uint16_t e_cs ; // offset 0x16
    uint16_t e_lfarlc ; // offset 0x18
    uint16_t e_ovno ; // offset 0x1a
    uint16_t e_res[4] ; // offset 0x1c
    uint16_t e_oemid ; // offset 0x24
    uint16_t e_oeminfo ; // offset 0x26
    uint16_t e_res2[10] ; // offset 0x28
    int32_t e_lfanew ; // offset 0x3c

} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER, **PPIMAGE_DOS_HEADER ;

typedef struct _PS_IMPERSONATION_INFORMATION { // 0xc bytes
    uint32_t Token ; // offset 0x0
    uint8_t CopyOnOpen ; // offset 0x4
    uint8_t EffectiveOnly ; // offset 0x5
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel ; // offset 0x8

} PS_IMPERSONATION_INFORMATION, *PPS_IMPERSONATION_INFORMATION, **PPPS_IMPERSONATION_INFORMATION ;

typedef struct _MMWSLE_HASH { // 0x8 bytes
    uint32_t Key ; // offset 0x0
    uint32_t Index ; // offset 0x4

} MMWSLE_HASH, *PMMWSLE_HASH, **PPMMWSLE_HASH ;

typedef struct _MMWSL { // 0x69c bytes
    uint32_t Quota ; // offset 0x0
    uint32_t FirstFree ; // offset 0x4
    uint32_t FirstDynamic ; // offset 0x8
    uint32_t LastEntry ; // offset 0xc
    uint32_t NextSlot ; // offset 0x10
    uint32_t Wsle ; // offset 0x14
    uint32_t LastInitializedWsle ; // offset 0x18
    uint32_t NonDirectCount ; // offset 0x1c
    uint32_t HashTable ; // offset 0x20
    uint32_t HashTableSize ; // offset 0x24
    uint32_t NumberOfCommittedPageTables ; // offset 0x28
    uint32_t HashTableStart ; // offset 0x2c
    uint32_t HighestPermittedHashAddress ; // offset 0x30
    uint32_t NumberOfImageWaiters ; // offset 0x34
    uint32_t VadBitMapHint ; // offset 0x38
    uint16_t UsedPageTableEntries[768] ; // offset 0x3c
    uint32_t CommittedPageTables[24] ; // offset 0x63c

} MMWSL, *PMMWSL, **PPMMWSL ;

typedef struct _CM_PARTIAL_RESOURCE_LIST { // 0x18 bytes
    uint16_t Version ; // offset 0x0
    uint16_t Revision ; // offset 0x2
    uint32_t Count ; // offset 0x4
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1] ; // offset 0x8

} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST, **PPCM_PARTIAL_RESOURCE_LIST ;

typedef struct _CM_FULL_RESOURCE_DESCRIPTOR { // 0x20 bytes
    INTERFACE_TYPE InterfaceType ; // offset 0x0
    uint32_t BusNumber ; // offset 0x4
    CM_PARTIAL_RESOURCE_LIST PartialResourceList ; // offset 0x8

} CM_FULL_RESOURCE_DESCRIPTOR, *PCM_FULL_RESOURCE_DESCRIPTOR, **PPCM_FULL_RESOURCE_DESCRIPTOR ;

typedef struct _CM_RESOURCE_LIST { // 0x24 bytes
    uint32_t Count ; // offset 0x0
    CM_FULL_RESOURCE_DESCRIPTOR List[1] ; // offset 0x4

} CM_RESOURCE_LIST, *PCM_RESOURCE_LIST, **PPCM_RESOURCE_LIST ;

typedef struct _PAGED_LOOKASIDE_LIST { // 0x100 bytes
    GENERAL_LOOKASIDE L ; // offset 0x0
    FAST_MUTEX Lock__ObsoleteButDoNotDelete ; // offset 0x80

} PAGED_LOOKASIDE_LIST, *PPAGED_LOOKASIDE_LIST, **PPPAGED_LOOKASIDE_LIST ;

typedef struct _ARBITER_CONFLICT_INFO { // 0x18 bytes
    uint32_t OwningObject ; // offset 0x0
    uint64_t Start ; // offset 0x8
    uint64_t End ; // offset 0x10

} ARBITER_CONFLICT_INFO, *PARBITER_CONFLICT_INFO, **PPARBITER_CONFLICT_INFO ;

typedef struct _ARBITER_PARAMETERS { // 0x10 bytes
    union {
        union {
        struct {
                    uint32_t ArbitrationList ; // offset 0x0
                    uint32_t AllocateFromCount ; // offset 0x4
                    uint32_t AllocateFrom ; // offset 0x8
                } TestAllocation ; // offset 0x0
        struct {
                    uint32_t ArbitrationList ; // offset 0x0
                    uint32_t AllocateFromCount ; // offset 0x4
                    uint32_t AllocateFrom ; // offset 0x8
                } RetestAllocation ; // offset 0x0
        struct {
                    uint32_t ArbitrationList ; // offset 0x0
                } BootAllocation ; // offset 0x0
        struct {
                    uint32_t AllocatedResources ; // offset 0x0
                } QueryAllocatedResources ; // offset 0x0
        struct {
                    uint32_t PhysicalDeviceObject ; // offset 0x0
                    uint32_t ConflictingResource ; // offset 0x4
                    uint32_t ConflictCount ; // offset 0x8
                    uint32_t Conflicts ; // offset 0xc
                } QueryConflict ; // offset 0x0
        struct {
                    uint32_t ArbitrationList ; // offset 0x0
                } QueryArbitrate ; // offset 0x0
        struct {
                    uint32_t ReserveDevice ; // offset 0x0
                } AddReserved ; // offset 0x0
        };
    } Parameters ; // offset 0x0

} ARBITER_PARAMETERS, *PARBITER_PARAMETERS, **PPARBITER_PARAMETERS ;

typedef struct _PCI_LOCK { // 0x8 bytes
    uint32_t Atom ; // offset 0x0
    uint8_t OldIrql ; // offset 0x4

} PCI_LOCK, *PPCI_LOCK, **PPPCI_LOCK ;

typedef struct _PCI_POWER_STATE { // 0x40 bytes
    SYSTEM_POWER_STATE CurrentSystemState ; // offset 0x0
    DEVICE_POWER_STATE CurrentDeviceState ; // offset 0x4
    SYSTEM_POWER_STATE SystemWakeLevel ; // offset 0x8
    DEVICE_POWER_STATE DeviceWakeLevel ; // offset 0xc
    DEVICE_POWER_STATE SystemStateMapping[7] ; // offset 0x10
    uint32_t WaitWakeIrp ; // offset 0x2c
    uint32_t SavedCancelRoutine ; // offset 0x30
    int32_t Paging ; // offset 0x34
    int32_t Hibernate ; // offset 0x38
    int32_t CrashDump ; // offset 0x3c

} PCI_POWER_STATE, *PPCI_POWER_STATE, **PPPCI_POWER_STATE ;

typedef struct _PCI_FDO_EXTENSION { // 0xc0 bytes
    SINGLE_LIST_ENTRY List ; // offset 0x0
    PCI_SIGNATURE ExtensionType ; // offset 0x4
    uint32_t IrpDispatchTable ; // offset 0x8
    uint8_t DeviceState ; // offset 0xc
    uint8_t TentativeNextState ; // offset 0xd
    KEVENT SecondaryExtLock ; // offset 0x10
    uint32_t PhysicalDeviceObject ; // offset 0x20
    uint32_t FunctionalDeviceObject ; // offset 0x24
    uint32_t AttachedDeviceObject ; // offset 0x28
    KEVENT ChildListLock ; // offset 0x2c
    uint32_t ChildPdoList ; // offset 0x3c
    uint32_t BusRootFdoExtension ; // offset 0x40
    uint32_t ParentFdoExtension ; // offset 0x44
    uint32_t ChildBridgePdoList ; // offset 0x48
    uint32_t PciBusInterface ; // offset 0x4c
    uint8_t MaxSubordinateBus ; // offset 0x50
    uint32_t BusHandler ; // offset 0x54
    uint8_t BaseBus ; // offset 0x58
    uint8_t Fake ; // offset 0x59
    uint8_t ChildDelete ; // offset 0x5a
    uint8_t Scanned ; // offset 0x5b
    uint8_t ArbitersInitialized ; // offset 0x5c
    uint8_t BrokenVideoHackApplied ; // offset 0x5d
    uint8_t Hibernated ; // offset 0x5e
    PCI_POWER_STATE PowerState ; // offset 0x60
    SINGLE_LIST_ENTRY SecondaryExtension ; // offset 0xa0
    uint32_t ChildWaitWakeCount ; // offset 0xa4
    uint32_t PreservedConfig ; // offset 0xa8
    PCI_LOCK Lock ; // offset 0xac
    struct {
        uint8_t Acquired ; // offset 0x0
        uint8_t CacheLineSize ; // offset 0x1
        uint8_t LatencyTimer ; // offset 0x2
        uint8_t EnablePERR ; // offset 0x3
        uint8_t EnableSERR ; // offset 0x4
    } HotPlugParameters ; // offset 0xb4
    uint32_t BusHackFlags ; // offset 0xbc

} PCI_FDO_EXTENSION, *PPCI_FDO_EXTENSION, **PPPCI_FDO_EXTENSION ;

typedef struct _PROCESSOR_POWER_POLICY_INFO { // 0x14 bytes
    uint32_t TimeCheck ; // offset 0x0
    uint32_t DemoteLimit ; // offset 0x4
    uint32_t PromoteLimit ; // offset 0x8
    uint8_t DemotePercent ; // offset 0xc
    uint8_t PromotePercent ; // offset 0xd
    uint8_t Spare[2] ; // offset 0xe
    union {
    uint32_t AllowDemotion : 1 ; // offset 0x10
    uint32_t AllowPromotion : 1 ; // offset 0x10
    uint32_t Reserved : 30 ; // offset 0x10
    };

} PROCESSOR_POWER_POLICY_INFO, *PPROCESSOR_POWER_POLICY_INFO, **PPPROCESSOR_POWER_POLICY_INFO ;

typedef struct _PROCESSOR_POWER_POLICY { // 0x4c bytes
    uint32_t Revision ; // offset 0x0
    uint8_t DynamicThrottle ; // offset 0x4
    uint8_t Spare[3] ; // offset 0x5
    union {
    uint32_t DisableCStates : 1 ; // offset 0x8
    uint32_t Reserved : 31 ; // offset 0x8
    };
    uint32_t PolicyCount ; // offset 0xc
    PROCESSOR_POWER_POLICY_INFO Policy[3] ; // offset 0x10

} PROCESSOR_POWER_POLICY, *PPROCESSOR_POWER_POLICY, **PPPROCESSOR_POWER_POLICY ;

typedef struct _INTERFACE { // 0x10 bytes
    uint16_t Size ; // offset 0x0
    uint16_t Version ; // offset 0x2
    uint32_t Context ; // offset 0x4
    uint32_t InterfaceReference ; // offset 0x8
    uint32_t InterfaceDereference ; // offset 0xc

} INTERFACE, *PINTERFACE, **PPINTERFACE ;

typedef struct _EX_PUSH_LOCK_WAIT_BLOCK { // 0x1c bytes
    KEVENT WakeEvent ; // offset 0x0
    uint32_t Next ; // offset 0x10
    uint32_t ShareCount ; // offset 0x14
    uint8_t Exclusive ; // offset 0x18

} EX_PUSH_LOCK_WAIT_BLOCK, *PEX_PUSH_LOCK_WAIT_BLOCK, **PPEX_PUSH_LOCK_WAIT_BLOCK ;

typedef struct _DBGKD_LOAD_SYMBOLS64 { // 0x28 bytes
    uint32_t PathNameLength ; // offset 0x0
    uint64_t BaseOfDll ; // offset 0x8
    uint64_t ProcessId ; // offset 0x10
    uint32_t CheckSum ; // offset 0x18
    uint32_t SizeOfImage ; // offset 0x1c
    uint8_t UnloadSymbols ; // offset 0x20

} DBGKD_LOAD_SYMBOLS64, *PDBGKD_LOAD_SYMBOLS64, **PPDBGKD_LOAD_SYMBOLS64 ;

typedef struct _OBJECT_SYMBOLIC_LINK { // 0x20 bytes
    LARGE_INTEGER CreationTime ; // offset 0x0
    UNICODE_STRING LinkTarget ; // offset 0x8
    UNICODE_STRING LinkTargetRemaining ; // offset 0x10
    uint32_t LinkTargetObject ; // offset 0x18
    uint32_t DosDeviceDriveIndex ; // offset 0x1c

} OBJECT_SYMBOLIC_LINK, *POBJECT_SYMBOLIC_LINK, **PPOBJECT_SYMBOLIC_LINK ;

typedef struct _HMAP_ENTRY { // 0x10 bytes
    uint32_t BlockAddress ; // offset 0x0
    uint32_t BinAddress ; // offset 0x4
    uint32_t CmView ; // offset 0x8
    uint32_t MemAlloc ; // offset 0xc

} HMAP_ENTRY, *PHMAP_ENTRY, **PPHMAP_ENTRY ;

typedef struct _HMAP_TABLE { // 0x2000 bytes
    HMAP_ENTRY Table[512] ; // offset 0x0

} HMAP_TABLE, *PHMAP_TABLE, **PPHMAP_TABLE ;

typedef struct _HMAP_DIRECTORY { // 0x1000 bytes
    uint32_t Directory[1024] ; // offset 0x0

} HMAP_DIRECTORY, *PHMAP_DIRECTORY, **PPHMAP_DIRECTORY ;

typedef struct _OBJECT_ATTRIBUTES { // 0x18 bytes
    uint32_t Length ; // offset 0x0
    uint32_t RootDirectory ; // offset 0x4
    uint32_t ObjectName ; // offset 0x8
    uint32_t Attributes ; // offset 0xc
    uint32_t SecurityDescriptor ; // offset 0x10
    uint32_t SecurityQualityOfService ; // offset 0x14

} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES, **PPOBJECT_ATTRIBUTES ;

typedef struct _POOL_HEADER { // 0x8 bytes
    union {
    uint16_t PreviousSize : 9 ; // offset 0x0
    uint16_t PoolIndex : 7 ; // offset 0x0
    };
    union {
    uint16_t BlockSize : 9 ; // offset 0x2
    uint16_t PoolType : 7 ; // offset 0x2
    };
    uint32_t Ulong1 ; // offset 0x0
    union {
    uint32_t ProcessBilled ; // offset 0x4
    uint32_t PoolTag ; // offset 0x4
    uint16_t AllocatorBackTraceIndex ; // offset 0x4
    };
    uint16_t PoolTagHash ; // offset 0x6

} POOL_HEADER, *PPOOL_HEADER, **PPPOOL_HEADER ;

typedef struct _POOL_HACKER { // 0x28 bytes
    POOL_HEADER Header ; // offset 0x0
    uint32_t Contents[8] ; // offset 0x8

} POOL_HACKER, *PPOOL_HACKER, **PPPOOL_HACKER ;

typedef struct _KMUTANT { // 0x20 bytes
    DISPATCHER_HEADER Header ; // offset 0x0
    LIST_ENTRY MutantListEntry ; // offset 0x10
    uint32_t OwnerThread ; // offset 0x18
    uint8_t Abandoned ; // offset 0x1c
    uint8_t ApcDisable ; // offset 0x1d

} KMUTANT, *PKMUTANT, **PPKMUTANT ;

typedef struct _PNP_DEVICE_EVENT_LIST { // 0x4c bytes
    int32_t Status ; // offset 0x0
    KMUTANT EventQueueMutex ; // offset 0x4
    FAST_MUTEX Lock ; // offset 0x24
    LIST_ENTRY List ; // offset 0x44

} PNP_DEVICE_EVENT_LIST, *PPNP_DEVICE_EVENT_LIST, **PPPNP_DEVICE_EVENT_LIST ;

typedef struct _SECTION_OBJECT { // 0x18 bytes
    uint32_t StartingVa ; // offset 0x0
    uint32_t EndingVa ; // offset 0x4
    uint32_t Parent ; // offset 0x8
    uint32_t LeftChild ; // offset 0xc
    uint32_t RightChild ; // offset 0x10
    uint32_t Segment ; // offset 0x14

} SECTION_OBJECT, *PSECTION_OBJECT, **PPSECTION_OBJECT ;

typedef struct _PRIVATE_CACHE_MAP_FLAGS { // 0x4 bytes
    union {
    uint32_t DontUse : 16 ; // offset 0x0
    uint32_t ReadAheadActive : 1 ; // offset 0x0
    uint32_t ReadAheadEnabled : 1 ; // offset 0x0
    uint32_t Available : 14 ; // offset 0x0
    };

} PRIVATE_CACHE_MAP_FLAGS, *PPRIVATE_CACHE_MAP_FLAGS, **PPPRIVATE_CACHE_MAP_FLAGS ;

typedef struct _PRIVATE_CACHE_MAP { // 0x58 bytes
    union {
    int16_t NodeTypeCode ; // offset 0x0
    PRIVATE_CACHE_MAP_FLAGS Flags ; // offset 0x0
    uint32_t UlongFlags ; // offset 0x0
    };
    uint32_t ReadAheadMask ; // offset 0x4
    uint32_t FileObject ; // offset 0x8
    LARGE_INTEGER FileOffset1 ; // offset 0x10
    LARGE_INTEGER BeyondLastByte1 ; // offset 0x18
    LARGE_INTEGER FileOffset2 ; // offset 0x20
    LARGE_INTEGER BeyondLastByte2 ; // offset 0x28
    LARGE_INTEGER ReadAheadOffset[2] ; // offset 0x30
    uint32_t ReadAheadLength[2] ; // offset 0x40
    uint32_t ReadAheadSpinLock ; // offset 0x48
    LIST_ENTRY PrivateLinks ; // offset 0x4c

} PRIVATE_CACHE_MAP, *PPRIVATE_CACHE_MAP, **PPPRIVATE_CACHE_MAP ;

typedef struct _VACB { // 0x18 bytes
    uint32_t BaseAddress ; // offset 0x0
    uint32_t SharedCacheMap ; // offset 0x4
    union {
        union {
        LARGE_INTEGER FileOffset ; // offset 0x0
        uint16_t ActiveCount ; // offset 0x0
        };
    } Overlay ; // offset 0x8
    LIST_ENTRY LruList ; // offset 0x10

} VACB, *PVACB, **PPVACB ;

typedef struct _SHARED_CACHE_MAP { // 0x130 bytes
    int16_t NodeTypeCode ; // offset 0x0
    int16_t NodeByteSize ; // offset 0x2
    uint32_t OpenCount ; // offset 0x4
    LARGE_INTEGER FileSize ; // offset 0x8
    LIST_ENTRY BcbList ; // offset 0x10
    LARGE_INTEGER SectionSize ; // offset 0x18
    LARGE_INTEGER ValidDataLength ; // offset 0x20
    LARGE_INTEGER ValidDataGoal ; // offset 0x28
    uint32_t InitialVacbs[4] ; // offset 0x30
    uint32_t Vacbs ; // offset 0x40
    uint32_t FileObject ; // offset 0x44
    uint32_t ActiveVacb ; // offset 0x48
    uint32_t NeedToZero ; // offset 0x4c
    uint32_t ActivePage ; // offset 0x50
    uint32_t NeedToZeroPage ; // offset 0x54
    uint32_t ActiveVacbSpinLock ; // offset 0x58
    uint32_t VacbActiveCount ; // offset 0x5c
    uint32_t DirtyPages ; // offset 0x60
    LIST_ENTRY SharedCacheMapLinks ; // offset 0x64
    uint32_t Flags ; // offset 0x6c
    int32_t Status ; // offset 0x70
    uint32_t Mbcb ; // offset 0x74
    uint32_t Section ; // offset 0x78
    uint32_t CreateEvent ; // offset 0x7c
    uint32_t WaitOnActiveCount ; // offset 0x80
    uint32_t PagesToWrite ; // offset 0x84
    int64_t BeyondLastFlush ; // offset 0x88
    uint32_t Callbacks ; // offset 0x90
    uint32_t LazyWriteContext ; // offset 0x94
    LIST_ENTRY PrivateList ; // offset 0x98
    uint32_t LogHandle ; // offset 0xa0
    uint32_t FlushToLsnRoutine ; // offset 0xa4
    uint32_t DirtyPageThreshold ; // offset 0xa8
    uint32_t LazyWritePassCount ; // offset 0xac
    uint32_t UninitializeEvent ; // offset 0xb0
    uint32_t NeedToZeroVacb ; // offset 0xb4
    uint32_t BcbSpinLock ; // offset 0xb8
    uint32_t Reserved ; // offset 0xbc
    KEVENT Event ; // offset 0xc0
    EX_PUSH_LOCK VacbPushLock ; // offset 0xd0
    PRIVATE_CACHE_MAP PrivateCacheMap ; // offset 0xd8

} SHARED_CACHE_MAP, *PSHARED_CACHE_MAP, **PPSHARED_CACHE_MAP ;

typedef struct _RTL_CRITICAL_SECTION_DEBUG { // 0x20 bytes
    uint16_t Type ; // offset 0x0
    uint16_t CreatorBackTraceIndex ; // offset 0x2
    uint32_t CriticalSection ; // offset 0x4
    LIST_ENTRY ProcessLocksList ; // offset 0x8
    uint32_t EntryCount ; // offset 0x10
    uint32_t ContentionCount ; // offset 0x14
    uint32_t Spare[2] ; // offset 0x18

} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG, **PPRTL_CRITICAL_SECTION_DEBUG ;

typedef struct _DEVICE_CAPABILITIES { // 0x40 bytes
    uint16_t Size ; // offset 0x0
    uint16_t Version ; // offset 0x2
    union {
    uint32_t DeviceD1 : 1 ; // offset 0x4
    uint32_t DeviceD2 : 1 ; // offset 0x4
    uint32_t LockSupported : 1 ; // offset 0x4
    uint32_t EjectSupported : 1 ; // offset 0x4
    uint32_t Removable : 1 ; // offset 0x4
    uint32_t DockDevice : 1 ; // offset 0x4
    uint32_t UniqueID : 1 ; // offset 0x4
    uint32_t SilentInstall : 1 ; // offset 0x4
    uint32_t RawDeviceOK : 1 ; // offset 0x4
    uint32_t SurpriseRemovalOK : 1 ; // offset 0x4
    uint32_t WakeFromD0 : 1 ; // offset 0x4
    uint32_t WakeFromD1 : 1 ; // offset 0x4
    uint32_t WakeFromD2 : 1 ; // offset 0x4
    uint32_t WakeFromD3 : 1 ; // offset 0x4
    uint32_t HardwareDisabled : 1 ; // offset 0x4
    uint32_t NonDynamic : 1 ; // offset 0x4
    uint32_t WarmEjectSupported : 1 ; // offset 0x4
    uint32_t NoDisplayInUI : 1 ; // offset 0x4
    uint32_t Reserved : 14 ; // offset 0x4
    };
    uint32_t Address ; // offset 0x8
    uint32_t UINumber ; // offset 0xc
    DEVICE_POWER_STATE DeviceState[7] ; // offset 0x10
    SYSTEM_POWER_STATE SystemWake ; // offset 0x2c
    DEVICE_POWER_STATE DeviceWake ; // offset 0x30
    uint32_t D1Latency ; // offset 0x34
    uint32_t D2Latency ; // offset 0x38
    uint32_t D3Latency ; // offset 0x3c

} DEVICE_CAPABILITIES, *PDEVICE_CAPABILITIES, **PPDEVICE_CAPABILITIES ;

typedef struct _HEAP_FREE_ENTRY_EXTRA { // 0x4 bytes
    uint16_t TagIndex ; // offset 0x0
    uint16_t FreeBackTraceIndex ; // offset 0x2

} HEAP_FREE_ENTRY_EXTRA, *PHEAP_FREE_ENTRY_EXTRA, **PPHEAP_FREE_ENTRY_EXTRA ;

typedef struct _LIST_ENTRY64 { // 0x10 bytes
    uint64_t Flink ; // offset 0x0
    uint64_t Blink ; // offset 0x8

} LIST_ENTRY64, *PLIST_ENTRY64, **PPLIST_ENTRY64 ;

typedef struct _GUID { // 0x10 bytes
    uint32_t Data1 ; // offset 0x0
    uint16_t Data2 ; // offset 0x4
    uint16_t Data3 ; // offset 0x6
    uint8_t Data4[8] ; // offset 0x8

} GUID, *PGUID, **PPGUID ;

typedef struct _DUMP_INITIALIZATION_CONTEXT { // 0x70 bytes
    uint32_t Length ; // offset 0x0
    uint32_t Reserved ; // offset 0x4
    uint32_t MemoryBlock ; // offset 0x8
    uint32_t CommonBuffer[2] ; // offset 0xc
    LARGE_INTEGER PhysicalAddress[2] ; // offset 0x18
    uint32_t StallRoutine ; // offset 0x28
    uint32_t OpenRoutine ; // offset 0x2c
    uint32_t WriteRoutine ; // offset 0x30
    uint32_t FinishRoutine ; // offset 0x34
    uint32_t AdapterObject ; // offset 0x38
    uint32_t MappedRegisterBase ; // offset 0x3c
    uint32_t PortConfiguration ; // offset 0x40
    uint8_t CrashDump ; // offset 0x44
    uint32_t MaximumTransferSize ; // offset 0x48
    uint32_t CommonBufferSize ; // offset 0x4c
    uint32_t TargetAddress ; // offset 0x50
    uint32_t WritePendingRoutine ; // offset 0x54
    uint32_t PartitionStyle ; // offset 0x58
    union {
        union {
        struct {
                    uint32_t Signature ; // offset 0x0
                    uint32_t CheckSum ; // offset 0x4
                } Mbr ; // offset 0x0
        struct {
                    GUID DiskId ; // offset 0x0
                } Gpt ; // offset 0x0
        };
    } DiskInfo ; // offset 0x5c

} DUMP_INITIALIZATION_CONTEXT, *PDUMP_INITIALIZATION_CONTEXT, **PPDUMP_INITIALIZATION_CONTEXT ;

typedef struct _DUMP_STACK_CONTEXT { // 0xb0 bytes
    DUMP_INITIALIZATION_CONTEXT Init ; // offset 0x0
    LARGE_INTEGER PartitionOffset ; // offset 0x70
    uint32_t DumpPointers ; // offset 0x78
    uint32_t PointersLength ; // offset 0x7c
    uint32_t ModulePrefix ; // offset 0x80
    LIST_ENTRY DriverList ; // offset 0x84
    STRING InitMsg ; // offset 0x8c
    STRING ProgMsg ; // offset 0x94
    STRING DoneMsg ; // offset 0x9c
    uint32_t FileObject ; // offset 0xa4
    DEVICE_USAGE_NOTIFICATION_TYPE UsageType ; // offset 0xa8

} DUMP_STACK_CONTEXT, *PDUMP_STACK_CONTEXT, **PPDUMP_STACK_CONTEXT ;

typedef struct _RTL_BITMAP { // 0x8 bytes
    uint32_t SizeOfBitMap ; // offset 0x0
    uint32_t Buffer ; // offset 0x4

} RTL_BITMAP, *PRTL_BITMAP, **PPRTL_BITMAP ;

typedef struct _POP_HIBER_CONTEXT { // 0xe0 bytes
    uint8_t WriteToFile ; // offset 0x0
    uint8_t ReserveLoaderMemory ; // offset 0x1
    uint8_t ReserveFreeMemory ; // offset 0x2
    uint8_t VerifyOnWake ; // offset 0x3
    uint8_t Reset ; // offset 0x4
    uint8_t HiberFlags ; // offset 0x5
    uint8_t LinkFile ; // offset 0x6
    uint32_t LinkFileHandle ; // offset 0x8
    uint32_t Lock ; // offset 0xc
    uint8_t MapFrozen ; // offset 0x10
    RTL_BITMAP MemoryMap ; // offset 0x14
    LIST_ENTRY ClonedRanges ; // offset 0x1c
    uint32_t ClonedRangeCount ; // offset 0x24
    uint32_t NextCloneRange ; // offset 0x28
    uint32_t NextPreserve ; // offset 0x2c
    uint32_t LoaderMdl ; // offset 0x30
    uint32_t Clones ; // offset 0x34
    uint32_t NextClone ; // offset 0x38
    uint32_t NoClones ; // offset 0x3c
    uint32_t Spares ; // offset 0x40
    uint64_t PagesOut ; // offset 0x48
    uint32_t IoPage ; // offset 0x50
    uint32_t CurrentMcb ; // offset 0x54
    uint32_t DumpStack ; // offset 0x58
    uint32_t WakeState ; // offset 0x5c
    uint32_t NoRanges ; // offset 0x60
    uint32_t HiberVa ; // offset 0x64
    LARGE_INTEGER HiberPte ; // offset 0x68
    int32_t Status ; // offset 0x70
    uint32_t MemoryImage ; // offset 0x74
    uint32_t TableHead ; // offset 0x78
    uint32_t CompressionWorkspace ; // offset 0x7c
    uint32_t CompressedWriteBuffer ; // offset 0x80
    uint32_t PerformanceStats ; // offset 0x84
    uint32_t CompressionBlock ; // offset 0x88
    uint32_t DmaIO ; // offset 0x8c
    uint32_t TemporaryHeap ; // offset 0x90
    PO_HIBER_PERF PerfInfo ; // offset 0x98

} POP_HIBER_CONTEXT, *PPOP_HIBER_CONTEXT, **PPPOP_HIBER_CONTEXT ;

typedef struct _EXCEPTION_RECORD { // 0x50 bytes
    int32_t ExceptionCode ; // offset 0x0
    uint32_t ExceptionFlags ; // offset 0x4
    uint32_t ExceptionRecord ; // offset 0x8
    uint32_t ExceptionAddress ; // offset 0xc
    uint32_t NumberParameters ; // offset 0x10
    uint32_t ExceptionInformation[15] ; // offset 0x14

} EXCEPTION_RECORD, *PEXCEPTION_RECORD, **PPEXCEPTION_RECORD ;

typedef struct _VI_DEADLOCK_THREAD { // 0x1c bytes
    uint32_t Thread ; // offset 0x0
    uint32_t CurrentSpinNode ; // offset 0x4
    uint32_t CurrentOtherNode ; // offset 0x8
    union {
    LIST_ENTRY ListEntry ; // offset 0xc
    LIST_ENTRY FreeListEntry ; // offset 0xc
    };
    uint32_t NodeCount ; // offset 0x14
    uint32_t PagingCount ; // offset 0x18

} VI_DEADLOCK_THREAD, *PVI_DEADLOCK_THREAD, **PPVI_DEADLOCK_THREAD ;

typedef struct _PROCESS_WS_WATCH_INFORMATION { // 0x8 bytes
    uint32_t FaultingPc ; // offset 0x0
    uint32_t FaultingVa ; // offset 0x4

} PROCESS_WS_WATCH_INFORMATION, *PPROCESS_WS_WATCH_INFORMATION, **PPPROCESS_WS_WATCH_INFORMATION ;

typedef struct _PAGEFAULT_HISTORY { // 0x18 bytes
    uint32_t CurrentIndex ; // offset 0x0
    uint32_t MaxIndex ; // offset 0x4
    uint32_t SpinLock ; // offset 0x8
    uint32_t Reserved ; // offset 0xc
    PROCESS_WS_WATCH_INFORMATION WatchInfo[1] ; // offset 0x10

} PAGEFAULT_HISTORY, *PPAGEFAULT_HISTORY, **PPPAGEFAULT_HISTORY ;

typedef struct _HEAP_PSEUDO_TAG_ENTRY { // 0xc bytes
    uint32_t Allocs ; // offset 0x0
    uint32_t Frees ; // offset 0x4
    uint32_t Size ; // offset 0x8

} HEAP_PSEUDO_TAG_ENTRY, *PHEAP_PSEUDO_TAG_ENTRY, **PPHEAP_PSEUDO_TAG_ENTRY ;

typedef struct _CM_KEY_BODY { // 0x44 bytes
    uint32_t Type ; // offset 0x0
    uint32_t KeyControlBlock ; // offset 0x4
    uint32_t NotifyBlock ; // offset 0x8
    uint32_t ProcessID ; // offset 0xc
    uint32_t Callers ; // offset 0x10
    uint32_t CallerAddress[10] ; // offset 0x14
    LIST_ENTRY KeyBodyList ; // offset 0x3c

} CM_KEY_BODY, *PCM_KEY_BODY, **PPCM_KEY_BODY ;

typedef struct _PHYSICAL_MEMORY_RUN { // 0x8 bytes
    uint32_t BasePage ; // offset 0x0
    uint32_t PageCount ; // offset 0x4

} PHYSICAL_MEMORY_RUN, *PPHYSICAL_MEMORY_RUN, **PPPHYSICAL_MEMORY_RUN ;

typedef struct _PHYSICAL_MEMORY_DESCRIPTOR { // 0x10 bytes
    uint32_t NumberOfRuns ; // offset 0x0
    uint32_t NumberOfPages ; // offset 0x4
    PHYSICAL_MEMORY_RUN Run[1] ; // offset 0x8

} PHYSICAL_MEMORY_DESCRIPTOR, *PPHYSICAL_MEMORY_DESCRIPTOR, **PPPHYSICAL_MEMORY_DESCRIPTOR ;

typedef struct _COMPRESSED_DATA_INFO { // 0xc bytes
    uint16_t CompressionFormatAndEngine ; // offset 0x0
    uint8_t CompressionUnitShift ; // offset 0x2
    uint8_t ChunkShift ; // offset 0x3
    uint8_t ClusterShift ; // offset 0x4
    uint8_t Reserved ; // offset 0x5
    uint16_t NumberOfChunks ; // offset 0x6
    uint32_t CompressedChunkSizes[1] ; // offset 0x8

} COMPRESSED_DATA_INFO, *PCOMPRESSED_DATA_INFO, **PPCOMPRESSED_DATA_INFO ;

typedef struct _POWER_ACTION_POLICY { // 0xc bytes
    POWER_ACTION Action ; // offset 0x0
    uint32_t Flags ; // offset 0x4
    uint32_t EventCode ; // offset 0x8

} POWER_ACTION_POLICY, *PPOWER_ACTION_POLICY, **PPPOWER_ACTION_POLICY ;

typedef struct _SYSTEM_POWER_LEVEL { // 0x18 bytes
    uint8_t Enable ; // offset 0x0
    uint8_t Spare[3] ; // offset 0x1
    uint32_t BatteryLevel ; // offset 0x4
    POWER_ACTION_POLICY PowerPolicy ; // offset 0x8
    SYSTEM_POWER_STATE MinSystemState ; // offset 0x14

} SYSTEM_POWER_LEVEL, *PSYSTEM_POWER_LEVEL, **PPSYSTEM_POWER_LEVEL ;

typedef struct _SYSTEM_POWER_POLICY { // 0xe8 bytes
    uint32_t Revision ; // offset 0x0
    POWER_ACTION_POLICY PowerButton ; // offset 0x4
    POWER_ACTION_POLICY SleepButton ; // offset 0x10
    POWER_ACTION_POLICY LidClose ; // offset 0x1c
    SYSTEM_POWER_STATE LidOpenWake ; // offset 0x28
    uint32_t Reserved ; // offset 0x2c
    POWER_ACTION_POLICY Idle ; // offset 0x30
    uint32_t IdleTimeout ; // offset 0x3c
    uint8_t IdleSensitivity ; // offset 0x40
    uint8_t DynamicThrottle ; // offset 0x41
    uint8_t Spare2[2] ; // offset 0x42
    SYSTEM_POWER_STATE MinSleep ; // offset 0x44
    SYSTEM_POWER_STATE MaxSleep ; // offset 0x48
    SYSTEM_POWER_STATE ReducedLatencySleep ; // offset 0x4c
    uint32_t WinLogonFlags ; // offset 0x50
    uint32_t Spare3 ; // offset 0x54
    uint32_t DozeS4Timeout ; // offset 0x58
    uint32_t BroadcastCapacityResolution ; // offset 0x5c
    SYSTEM_POWER_LEVEL DischargePolicy[4] ; // offset 0x60
    uint32_t VideoTimeout ; // offset 0xc0
    uint8_t VideoDimDisplay ; // offset 0xc4
    uint32_t VideoReserved[3] ; // offset 0xc8
    uint32_t SpindownTimeout ; // offset 0xd4
    uint8_t OptimizeForPower ; // offset 0xd8
    uint8_t FanThrottleTolerance ; // offset 0xd9
    uint8_t ForcedThrottle ; // offset 0xda
    uint8_t MinThrottle ; // offset 0xdb
    POWER_ACTION_POLICY OverThrottled ; // offset 0xdc

} SYSTEM_POWER_POLICY, *PSYSTEM_POWER_POLICY, **PPSYSTEM_POWER_POLICY ;

typedef struct _PCI_COMMON_EXTENSION { // 0x20 bytes
    uint32_t Next ; // offset 0x0
    PCI_SIGNATURE ExtensionType ; // offset 0x4
    uint32_t IrpDispatchTable ; // offset 0x8
    uint8_t DeviceState ; // offset 0xc
    uint8_t TentativeNextState ; // offset 0xd
    KEVENT SecondaryExtLock ; // offset 0x10

} PCI_COMMON_EXTENSION, *PPCI_COMMON_EXTENSION, **PPPCI_COMMON_EXTENSION ;

typedef struct _EXCEPTION_POINTERS { // 0x8 bytes
    uint32_t ExceptionRecord ; // offset 0x0
    uint32_t ContextRecord ; // offset 0x4

} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS, **PPEXCEPTION_POINTERS ;

typedef struct _PEB { // 0x210 bytes
    uint8_t InheritedAddressSpace ; // offset 0x0
    uint8_t ReadImageFileExecOptions ; // offset 0x1
    uint8_t BeingDebugged ; // offset 0x2
    uint8_t SpareBool ; // offset 0x3
    uint32_t Mutant ; // offset 0x4
    uint32_t ImageBaseAddress ; // offset 0x8
    uint32_t Ldr ; // offset 0xc
    uint32_t ProcessParameters ; // offset 0x10
    uint32_t SubSystemData ; // offset 0x14
    uint32_t ProcessHeap ; // offset 0x18
    uint32_t FastPebLock ; // offset 0x1c
    uint32_t FastPebLockRoutine ; // offset 0x20
    uint32_t FastPebUnlockRoutine ; // offset 0x24
    uint32_t EnvironmentUpdateCount ; // offset 0x28
    uint32_t KernelCallbackTable ; // offset 0x2c
    uint32_t SystemReserved[1] ; // offset 0x30
    uint32_t AtlThunkSListPtr32 ; // offset 0x34
    uint32_t FreeList ; // offset 0x38
    uint32_t TlsExpansionCounter ; // offset 0x3c
    uint32_t TlsBitmap ; // offset 0x40
    uint32_t TlsBitmapBits[2] ; // offset 0x44
    uint32_t ReadOnlySharedMemoryBase ; // offset 0x4c
    uint32_t ReadOnlySharedMemoryHeap ; // offset 0x50
    uint32_t ReadOnlyStaticServerData ; // offset 0x54
    uint32_t AnsiCodePageData ; // offset 0x58
    uint32_t OemCodePageData ; // offset 0x5c
    uint32_t UnicodeCaseTableData ; // offset 0x60
    uint32_t NumberOfProcessors ; // offset 0x64
    uint32_t NtGlobalFlag ; // offset 0x68
    LARGE_INTEGER CriticalSectionTimeout ; // offset 0x70
    uint32_t HeapSegmentReserve ; // offset 0x78
    uint32_t HeapSegmentCommit ; // offset 0x7c
    uint32_t HeapDeCommitTotalFreeThreshold ; // offset 0x80
    uint32_t HeapDeCommitFreeBlockThreshold ; // offset 0x84
    uint32_t NumberOfHeaps ; // offset 0x88
    uint32_t MaximumNumberOfHeaps ; // offset 0x8c
    uint32_t ProcessHeaps ; // offset 0x90
    uint32_t GdiSharedHandleTable ; // offset 0x94
    uint32_t ProcessStarterHelper ; // offset 0x98
    uint32_t GdiDCAttributeList ; // offset 0x9c
    uint32_t LoaderLock ; // offset 0xa0
    uint32_t OSMajorVersion ; // offset 0xa4
    uint32_t OSMinorVersion ; // offset 0xa8
    uint16_t OSBuildNumber ; // offset 0xac
    uint16_t OSCSDVersion ; // offset 0xae
    uint32_t OSPlatformId ; // offset 0xb0
    uint32_t ImageSubsystem ; // offset 0xb4
    uint32_t ImageSubsystemMajorVersion ; // offset 0xb8
    uint32_t ImageSubsystemMinorVersion ; // offset 0xbc
    uint32_t ImageProcessAffinityMask ; // offset 0xc0
    uint32_t GdiHandleBuffer[34] ; // offset 0xc4
    uint32_t PostProcessInitRoutine ; // offset 0x14c
    uint32_t TlsExpansionBitmap ; // offset 0x150
    uint32_t TlsExpansionBitmapBits[32] ; // offset 0x154
    uint32_t SessionId ; // offset 0x1d4
    ULARGE_INTEGER AppCompatFlags ; // offset 0x1d8
    ULARGE_INTEGER AppCompatFlagsUser ; // offset 0x1e0
    uint32_t pShimData ; // offset 0x1e8
    uint32_t AppCompatInfo ; // offset 0x1ec
    UNICODE_STRING CSDVersion ; // offset 0x1f0
    uint32_t ActivationContextData ; // offset 0x1f8
    uint32_t ProcessAssemblyStorageMap ; // offset 0x1fc
    uint32_t SystemDefaultActivationContextData ; // offset 0x200
    uint32_t SystemAssemblyStorageMap ; // offset 0x204
    uint32_t MinimumStackCommit ; // offset 0x208

} PEB, *PPEB, **PPPEB ;

typedef struct _OBJECT_DUMP_CONTROL { // 0x8 bytes
    uint32_t Stream ; // offset 0x0
    uint32_t Detail ; // offset 0x4

} OBJECT_DUMP_CONTROL, *POBJECT_DUMP_CONTROL, **PPOBJECT_DUMP_CONTROL ;

typedef struct _HEAP_UCR_SEGMENT { // 0x10 bytes
    uint32_t Next ; // offset 0x0
    uint32_t ReservedSize ; // offset 0x4
    uint32_t CommittedSize ; // offset 0x8
    uint32_t filler ; // offset 0xc

} HEAP_UCR_SEGMENT, *PHEAP_UCR_SEGMENT, **PPHEAP_UCR_SEGMENT ;

typedef struct _EPROCESS_QUOTA_ENTRY { // 0x10 bytes
    uint32_t Usage ; // offset 0x0
    uint32_t Limit ; // offset 0x4
    uint32_t Peak ; // offset 0x8
    uint32_t Return ; // offset 0xc

} EPROCESS_QUOTA_ENTRY, *PEPROCESS_QUOTA_ENTRY, **PPEPROCESS_QUOTA_ENTRY ;

typedef struct _EPROCESS_QUOTA_BLOCK { // 0x40 bytes
    EPROCESS_QUOTA_ENTRY QuotaEntry[3] ; // offset 0x0
    LIST_ENTRY QuotaList ; // offset 0x30
    uint32_t ReferenceCount ; // offset 0x38
    uint32_t ProcessCount ; // offset 0x3c

} EPROCESS_QUOTA_BLOCK, *PEPROCESS_QUOTA_BLOCK, **PPEPROCESS_QUOTA_BLOCK ;

typedef struct _POWER_SEQUENCE { // 0xc bytes
    uint32_t SequenceD1 ; // offset 0x0
    uint32_t SequenceD2 ; // offset 0x4
    uint32_t SequenceD3 ; // offset 0x8

} POWER_SEQUENCE, *PPOWER_SEQUENCE, **PPPOWER_SEQUENCE ;

typedef struct _FAST_IO_DISPATCH { // 0x70 bytes
    uint32_t SizeOfFastIoDispatch ; // offset 0x0
    uint32_t FastIoCheckIfPossible ; // offset 0x4
    uint32_t FastIoRead ; // offset 0x8
    uint32_t FastIoWrite ; // offset 0xc
    uint32_t FastIoQueryBasicInfo ; // offset 0x10
    uint32_t FastIoQueryStandardInfo ; // offset 0x14
    uint32_t FastIoLock ; // offset 0x18
    uint32_t FastIoUnlockSingle ; // offset 0x1c
    uint32_t FastIoUnlockAll ; // offset 0x20
    uint32_t FastIoUnlockAllByKey ; // offset 0x24
    uint32_t FastIoDeviceControl ; // offset 0x28
    uint32_t AcquireFileForNtCreateSection ; // offset 0x2c
    uint32_t ReleaseFileForNtCreateSection ; // offset 0x30
    uint32_t FastIoDetachDevice ; // offset 0x34
    uint32_t FastIoQueryNetworkOpenInfo ; // offset 0x38
    uint32_t AcquireForModWrite ; // offset 0x3c
    uint32_t MdlRead ; // offset 0x40
    uint32_t MdlReadComplete ; // offset 0x44
    uint32_t PrepareMdlWrite ; // offset 0x48
    uint32_t MdlWriteComplete ; // offset 0x4c
    uint32_t FastIoReadCompressed ; // offset 0x50
    uint32_t FastIoWriteCompressed ; // offset 0x54
    uint32_t MdlReadCompleteCompressed ; // offset 0x58
    uint32_t MdlWriteCompleteCompressed ; // offset 0x5c
    uint32_t FastIoQueryOpen ; // offset 0x60
    uint32_t ReleaseForModWrite ; // offset 0x64
    uint32_t AcquireForCcFlush ; // offset 0x68
    uint32_t ReleaseForCcFlush ; // offset 0x6c

} FAST_IO_DISPATCH, *PFAST_IO_DISPATCH, **PPFAST_IO_DISPATCH ;

typedef struct _RTL_HANDLE_TABLE_ENTRY { // 0x4 bytes
    union {
    uint32_t Flags ; // offset 0x0
    uint32_t NextFree ; // offset 0x0
    };

} RTL_HANDLE_TABLE_ENTRY, *PRTL_HANDLE_TABLE_ENTRY, **PPRTL_HANDLE_TABLE_ENTRY ;

typedef struct _ACL { // 0x8 bytes
    uint8_t AclRevision ; // offset 0x0
    uint8_t Sbz1 ; // offset 0x1
    uint16_t AclSize ; // offset 0x2
    uint16_t AceCount ; // offset 0x4
    uint16_t Sbz2 ; // offset 0x6

} ACL, *PACL, **PPACL ;

typedef struct _CM_KEY_SECURITY_CACHE_ENTRY { // 0x8 bytes
    uint32_t Cell ; // offset 0x0
    uint32_t CachedSecurity ; // offset 0x4

} CM_KEY_SECURITY_CACHE_ENTRY, *PCM_KEY_SECURITY_CACHE_ENTRY, **PPCM_KEY_SECURITY_CACHE_ENTRY ;

typedef struct _CM_VIEW_OF_FILE { // 0x24 bytes
    LIST_ENTRY LRUViewList ; // offset 0x0
    LIST_ENTRY PinViewList ; // offset 0x8
    uint32_t FileOffset ; // offset 0x10
    uint32_t Size ; // offset 0x14
    uint32_t ViewAddress ; // offset 0x18
    uint32_t Bcb ; // offset 0x1c
    uint32_t UseCount ; // offset 0x20

} CM_VIEW_OF_FILE, *PCM_VIEW_OF_FILE, **PPCM_VIEW_OF_FILE ;

typedef struct _IRP { // 0x70 bytes
    int16_t Type ; // offset 0x0
    uint16_t Size ; // offset 0x2
    uint32_t MdlAddress ; // offset 0x4
    uint32_t Flags ; // offset 0x8
    union {
        union {
        uint32_t MasterIrp ; // offset 0x0
        int32_t IrpCount ; // offset 0x0
        uint32_t SystemBuffer ; // offset 0x0
        };
    } AssociatedIrp ; // offset 0xc
    LIST_ENTRY ThreadListEntry ; // offset 0x10
    IO_STATUS_BLOCK IoStatus ; // offset 0x18
    uint8_t RequestorMode ; // offset 0x20
    uint8_t PendingReturned ; // offset 0x21
    uint8_t StackCount ; // offset 0x22
    uint8_t CurrentLocation ; // offset 0x23
    uint8_t Cancel ; // offset 0x24
    uint8_t CancelIrql ; // offset 0x25
    uint8_t ApcEnvironment ; // offset 0x26
    uint8_t AllocationFlags ; // offset 0x27
    uint32_t UserIosb ; // offset 0x28
    uint32_t UserEvent ; // offset 0x2c
    union {
        union {
        struct {
                    uint32_t UserApcRoutine ; // offset 0x0
                    uint32_t UserApcContext ; // offset 0x4
                } AsynchronousParameters ; // offset 0x0
        LARGE_INTEGER AllocationSize ; // offset 0x0
        };
    } Overlay ; // offset 0x30
    uint32_t CancelRoutine ; // offset 0x38
    uint32_t UserBuffer ; // offset 0x3c
    union {
        union {
        struct {
                    union {
                    KDEVICE_QUEUE_ENTRY DeviceQueueEntry ; // offset 0x0
                    uint32_t DriverContext[4] ; // offset 0x0
                    };
                    uint32_t Thread ; // offset 0x10
                    uint32_t AuxiliaryBuffer ; // offset 0x14
                    LIST_ENTRY ListEntry ; // offset 0x18
                    union {
                    uint32_t CurrentStackLocation ; // offset 0x20
                    uint32_t PacketType ; // offset 0x20
                    };
                    uint32_t OriginalFileObject ; // offset 0x24
                } Overlay ; // offset 0x0
        KAPC Apc ; // offset 0x0
        uint32_t CompletionKey ; // offset 0x0
        };
    } Tail ; // offset 0x40

} IRP, *PIRP, **PPIRP ;

typedef struct _CALL_PERFORMANCE_DATA { // 0x204 bytes
    uint32_t SpinLock ; // offset 0x0
    LIST_ENTRY HashTable[64] ; // offset 0x4

} CALL_PERFORMANCE_DATA, *PCALL_PERFORMANCE_DATA, **PPCALL_PERFORMANCE_DATA ;

typedef union _PCI_HEADER_TYPE_DEPENDENT { // 0x4 bytes
    union {
    struct {
            uint8_t Spare[4] ; // offset 0x0
        } type0 ; // offset 0x0
    struct {
            uint8_t PrimaryBus ; // offset 0x0
            uint8_t SecondaryBus ; // offset 0x1
            uint8_t SubordinateBus ; // offset 0x2
            union {
            uint8_t SubtractiveDecode : 1 ; // offset 0x3
            uint8_t IsaBitSet : 1 ; // offset 0x3
            uint8_t VgaBitSet : 1 ; // offset 0x3
            uint8_t WeChangedBusNumbers : 1 ; // offset 0x3
            uint8_t IsaBitRequired : 1 ; // offset 0x3
            };
        } type1 ; // offset 0x0
    struct {
            uint8_t PrimaryBus ; // offset 0x0
            uint8_t SecondaryBus ; // offset 0x1
            uint8_t SubordinateBus ; // offset 0x2
            union {
            uint8_t SubtractiveDecode : 1 ; // offset 0x3
            uint8_t IsaBitSet : 1 ; // offset 0x3
            uint8_t VgaBitSet : 1 ; // offset 0x3
            uint8_t WeChangedBusNumbers : 1 ; // offset 0x3
            uint8_t IsaBitRequired : 1 ; // offset 0x3
            };
        } type2 ; // offset 0x0
    };

} PCI_HEADER_TYPE_DEPENDENT, *PPCI_HEADER_TYPE_DEPENDENT, **PPPCI_HEADER_TYPE_DEPENDENT ;

typedef struct _PCI_SLOT_NUMBER { // 0x4 bytes
    union {
        union {
        struct {
                    union {
                    uint32_t DeviceNumber : 5 ; // offset 0x0
                    uint32_t FunctionNumber : 3 ; // offset 0x0
                    uint32_t Reserved : 24 ; // offset 0x0
                    };
                } bits ; // offset 0x0
        uint32_t AsULONG ; // offset 0x0
        };
    } u ; // offset 0x0

} PCI_SLOT_NUMBER, *PPCI_SLOT_NUMBER, **PPPCI_SLOT_NUMBER ;

typedef struct _PM_SUPPORT { // 0x1 bytes
    union {
    uint8_t Rsvd2 : 1 ; // offset 0x0
    uint8_t D1 : 1 ; // offset 0x0
    uint8_t D2 : 1 ; // offset 0x0
    uint8_t PMED0 : 1 ; // offset 0x0
    uint8_t PMED1 : 1 ; // offset 0x0
    uint8_t PMED2 : 1 ; // offset 0x0
    uint8_t PMED3Hot : 1 ; // offset 0x0
    uint8_t PMED3Cold : 1 ; // offset 0x0
    };

} PM_SUPPORT, *PPM_SUPPORT, **PPPM_SUPPORT ;

typedef struct _PCI_PMC { // 0x2 bytes
    union {
    uint8_t Version : 3 ; // offset 0x0
    uint8_t PMEClock : 1 ; // offset 0x0
    uint8_t Rsvd1 : 1 ; // offset 0x0
    uint8_t DeviceSpecificInitialization : 1 ; // offset 0x0
    uint8_t Rsvd2 : 2 ; // offset 0x0
    };
    PM_SUPPORT Support ; // offset 0x1

} PCI_PMC, *PPCI_PMC, **PPPCI_PMC ;

typedef struct _PCI_PDO_EXTENSION { // 0xc8 bytes
    uint32_t Next ; // offset 0x0
    PCI_SIGNATURE ExtensionType ; // offset 0x4
    uint32_t IrpDispatchTable ; // offset 0x8
    uint8_t DeviceState ; // offset 0xc
    uint8_t TentativeNextState ; // offset 0xd
    KEVENT SecondaryExtLock ; // offset 0x10
    PCI_SLOT_NUMBER Slot ; // offset 0x20
    uint32_t PhysicalDeviceObject ; // offset 0x24
    uint32_t ParentFdoExtension ; // offset 0x28
    SINGLE_LIST_ENTRY SecondaryExtension ; // offset 0x2c
    uint32_t BusInterfaceReferenceCount ; // offset 0x30
    uint32_t AgpInterfaceReferenceCount ; // offset 0x34
    uint16_t VendorId ; // offset 0x38
    uint16_t DeviceId ; // offset 0x3a
    uint16_t SubsystemVendorId ; // offset 0x3c
    uint16_t SubsystemId ; // offset 0x3e
    uint8_t RevisionId ; // offset 0x40
    uint8_t ProgIf ; // offset 0x41
    uint8_t SubClass ; // offset 0x42
    uint8_t BaseClass ; // offset 0x43
    uint8_t AdditionalResourceCount ; // offset 0x44
    uint8_t AdjustedInterruptLine ; // offset 0x45
    uint8_t InterruptPin ; // offset 0x46
    uint8_t RawInterruptLine ; // offset 0x47
    uint8_t CapabilitiesPtr ; // offset 0x48
    uint8_t SavedLatencyTimer ; // offset 0x49
    uint8_t SavedCacheLineSize ; // offset 0x4a
    uint8_t HeaderType ; // offset 0x4b
    uint8_t NotPresent ; // offset 0x4c
    uint8_t ReportedMissing ; // offset 0x4d
    uint8_t ExpectedWritebackFailure ; // offset 0x4e
    uint8_t NoTouchPmeEnable ; // offset 0x4f
    uint8_t LegacyDriver ; // offset 0x50
    uint8_t UpdateHardware ; // offset 0x51
    uint8_t MovedDevice ; // offset 0x52
    uint8_t DisablePowerDown ; // offset 0x53
    uint8_t NeedsHotPlugConfiguration ; // offset 0x54
    uint8_t SwitchedIDEToNativeMode ; // offset 0x55
    uint8_t BIOSAllowsIDESwitchToNativeMode ; // offset 0x56
    uint8_t IoSpaceUnderNativeIdeControl ; // offset 0x57
    uint8_t OnDebugPath ; // offset 0x58
    PCI_POWER_STATE PowerState ; // offset 0x5c
    PCI_HEADER_TYPE_DEPENDENT Dependent ; // offset 0x9c
    uint64_t HackFlags ; // offset 0xa0
    uint32_t Resources ; // offset 0xa8
    uint32_t BridgeFdoExtension ; // offset 0xac
    uint32_t NextBridge ; // offset 0xb0
    uint32_t NextHashEntry ; // offset 0xb4
    PCI_LOCK Lock ; // offset 0xb8
    PCI_PMC PowerCapabilities ; // offset 0xc0
    uint8_t TargetAgpCapabilityId ; // offset 0xc2
    uint16_t CommandEnables ; // offset 0xc4
    uint16_t InitialCommand ; // offset 0xc6

} PCI_PDO_EXTENSION, *PPCI_PDO_EXTENSION, **PPPCI_PDO_EXTENSION ;

typedef struct _OBJECT_HANDLE_INFORMATION { // 0x8 bytes
    uint32_t HandleAttributes ; // offset 0x0
    uint32_t GrantedAccess ; // offset 0x4

} OBJECT_HANDLE_INFORMATION, *POBJECT_HANDLE_INFORMATION, **PPOBJECT_HANDLE_INFORMATION ;

typedef struct _ARBITER_INTERFACE { // 0x18 bytes
    uint16_t Size ; // offset 0x0
    uint16_t Version ; // offset 0x2
    uint32_t Context ; // offset 0x4
    uint32_t InterfaceReference ; // offset 0x8
    uint32_t InterfaceDereference ; // offset 0xc
    uint32_t ArbiterHandler ; // offset 0x10
    uint32_t Flags ; // offset 0x14

} ARBITER_INTERFACE, *PARBITER_INTERFACE, **PPARBITER_INTERFACE ;

typedef struct _HANDLE_TABLE_ENTRY { // 0x8 bytes
    union {
    uint32_t Object ; // offset 0x0
    uint32_t ObAttributes ; // offset 0x0
    uint32_t InfoTable ; // offset 0x0
    uint32_t Value ; // offset 0x0
    };
    union {
    uint32_t GrantedAccess ; // offset 0x4
    uint16_t GrantedAccessIndex ; // offset 0x4
    };
    uint16_t CreatorBackTraceIndex ; // offset 0x6
    int32_t NextFreeTableEntry ; // offset 0x4

} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY, **PPHANDLE_TABLE_ENTRY ;

typedef struct _TEB_ACTIVE_FRAME { // 0xc bytes
    uint32_t Flags ; // offset 0x0
    uint32_t Previous ; // offset 0x4
    uint32_t Context ; // offset 0x8

} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME, **PPTEB_ACTIVE_FRAME ;

typedef struct _LDR_DATA_TABLE_ENTRY { // 0x50 bytes
    LIST_ENTRY InLoadOrderLinks ; // offset 0x0
    LIST_ENTRY InMemoryOrderLinks ; // offset 0x8
    LIST_ENTRY InInitializationOrderLinks ; // offset 0x10
    uint32_t DllBase ; // offset 0x18
    uint32_t EntryPoint ; // offset 0x1c
    uint32_t SizeOfImage ; // offset 0x20
    UNICODE_STRING FullDllName ; // offset 0x24
    UNICODE_STRING BaseDllName ; // offset 0x2c
    uint32_t Flags ; // offset 0x34
    uint16_t LoadCount ; // offset 0x38
    uint16_t TlsIndex ; // offset 0x3a
    union {
    LIST_ENTRY HashLinks ; // offset 0x3c
    uint32_t SectionPointer ; // offset 0x3c
    };
    uint32_t CheckSum ; // offset 0x40
    union {
    uint32_t TimeDateStamp ; // offset 0x44
    uint32_t LoadedImports ; // offset 0x44
    };
    uint32_t EntryPointActivationContext ; // offset 0x48
    uint32_t PatchInformation ; // offset 0x4c

} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY, **PPLDR_DATA_TABLE_ENTRY ;

typedef struct _MMVIEW { // 0x8 bytes
    uint32_t Entry ; // offset 0x0
    uint32_t ControlArea ; // offset 0x4

} MMVIEW, *PMMVIEW, **PPMMVIEW ;

typedef struct _VI_DEADLOCK_NODE { // 0x68 bytes
    uint32_t Parent ; // offset 0x0
    LIST_ENTRY ChildrenList ; // offset 0x4
    LIST_ENTRY SiblingsList ; // offset 0xc
    union {
    LIST_ENTRY ResourceList ; // offset 0x14
    LIST_ENTRY FreeListEntry ; // offset 0x14
    };
    uint32_t Root ; // offset 0x1c
    uint32_t ThreadEntry ; // offset 0x20
    union {
    uint32_t Active : 1 ; // offset 0x24
    uint32_t OnlyTryAcquireUsed : 1 ; // offset 0x24
    uint32_t SequenceNumber : 30 ; // offset 0x24
    };
    uint32_t StackTrace[8] ; // offset 0x28
    uint32_t ParentStackTrace[8] ; // offset 0x48

} VI_DEADLOCK_NODE, *PVI_DEADLOCK_NODE, **PPVI_DEADLOCK_NODE ;

typedef struct _VI_DEADLOCK_GLOBALS { // 0x110 bytes
    uint32_t Nodes[2] ; // offset 0x0
    uint32_t Resources[2] ; // offset 0x8
    uint32_t Threads[2] ; // offset 0x10
    int64_t TimeAcquire ; // offset 0x18
    int64_t TimeRelease ; // offset 0x20
    uint32_t BytesAllocated ; // offset 0x28
    uint32_t ResourceDatabase ; // offset 0x2c
    uint32_t ThreadDatabase ; // offset 0x30
    uint32_t AllocationFailures ; // offset 0x34
    uint32_t NodesTrimmedBasedOnAge ; // offset 0x38
    uint32_t NodesTrimmedBasedOnCount ; // offset 0x3c
    uint32_t NodesSearched ; // offset 0x40
    uint32_t MaxNodesSearched ; // offset 0x44
    uint32_t SequenceNumber ; // offset 0x48
    uint32_t RecursionDepthLimit ; // offset 0x4c
    uint32_t SearchedNodesLimit ; // offset 0x50
    uint32_t DepthLimitHits ; // offset 0x54
    uint32_t SearchLimitHits ; // offset 0x58
    uint32_t ABC_ACB_Skipped ; // offset 0x5c
    LIST_ENTRY FreeResourceList ; // offset 0x60
    LIST_ENTRY FreeThreadList ; // offset 0x68
    LIST_ENTRY FreeNodeList ; // offset 0x70
    uint32_t FreeResourceCount ; // offset 0x78
    uint32_t FreeThreadCount ; // offset 0x7c
    uint32_t FreeNodeCount ; // offset 0x80
    uint32_t Instigator ; // offset 0x84
    uint32_t NumberOfParticipants ; // offset 0x88
    uint32_t Participant[32] ; // offset 0x8c
    uint32_t CacheReductionInProgress ; // offset 0x10c

} VI_DEADLOCK_GLOBALS, *PVI_DEADLOCK_GLOBALS, **PPVI_DEADLOCK_GLOBALS ;

typedef struct _BUS_EXTENSION_LIST { // 0x8 bytes
    uint32_t Next ; // offset 0x0
    uint32_t BusExtension ; // offset 0x4

} BUS_EXTENSION_LIST, *PBUS_EXTENSION_LIST, **PPBUS_EXTENSION_LIST ;

typedef struct _CACHE_UNINITIALIZE_EVENT { // 0x14 bytes
    uint32_t Next ; // offset 0x0
    KEVENT Event ; // offset 0x4

} CACHE_UNINITIALIZE_EVENT, *PCACHE_UNINITIALIZE_EVENT, **PPCACHE_UNINITIALIZE_EVENT ;

typedef struct _MI_VERIFIER_POOL_HEADER { // 0x8 bytes
    uint32_t ListIndex ; // offset 0x0
    uint32_t Verifier ; // offset 0x4

} MI_VERIFIER_POOL_HEADER, *PMI_VERIFIER_POOL_HEADER, **PPMI_VERIFIER_POOL_HEADER ;

typedef struct _FS_FILTER_CALLBACKS { // 0x38 bytes
    uint32_t SizeOfFsFilterCallbacks ; // offset 0x0
    uint32_t Reserved ; // offset 0x4
    uint32_t PreAcquireForSectionSynchronization ; // offset 0x8
    uint32_t PostAcquireForSectionSynchronization ; // offset 0xc
    uint32_t PreReleaseForSectionSynchronization ; // offset 0x10
    uint32_t PostReleaseForSectionSynchronization ; // offset 0x14
    uint32_t PreAcquireForCcFlush ; // offset 0x18
    uint32_t PostAcquireForCcFlush ; // offset 0x1c
    uint32_t PreReleaseForCcFlush ; // offset 0x20
    uint32_t PostReleaseForCcFlush ; // offset 0x24
    uint32_t PreAcquireForModifiedPageWriter ; // offset 0x28
    uint32_t PostAcquireForModifiedPageWriter ; // offset 0x2c
    uint32_t PreReleaseForModifiedPageWriter ; // offset 0x30
    uint32_t PostReleaseForModifiedPageWriter ; // offset 0x34

} FS_FILTER_CALLBACKS, *PFS_FILTER_CALLBACKS, **PPFS_FILTER_CALLBACKS ;

typedef struct _SECTION_IMAGE_INFORMATION { // 0x30 bytes
    uint32_t TransferAddress ; // offset 0x0
    uint32_t ZeroBits ; // offset 0x4
    uint32_t MaximumStackSize ; // offset 0x8
    uint32_t CommittedStackSize ; // offset 0xc
    uint32_t SubSystemType ; // offset 0x10
    uint16_t SubSystemMinorVersion ; // offset 0x14
    uint16_t SubSystemMajorVersion ; // offset 0x16
    uint32_t SubSystemVersion ; // offset 0x14
    uint32_t GpValue ; // offset 0x18
    uint16_t ImageCharacteristics ; // offset 0x1c
    uint16_t DllCharacteristics ; // offset 0x1e
    uint16_t Machine ; // offset 0x20
    uint8_t ImageContainsCode ; // offset 0x22
    uint8_t Spare1 ; // offset 0x23
    uint32_t LoaderFlags ; // offset 0x24
    uint32_t ImageFileSize ; // offset 0x28
    uint32_t Reserved[1] ; // offset 0x2c

} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION, **PPSECTION_IMAGE_INFORMATION ;

typedef struct _POP_DEVICE_POWER_IRP { // 0x2c bytes
    SINGLE_LIST_ENTRY Free ; // offset 0x0
    uint32_t Irp ; // offset 0x4
    uint32_t Notify ; // offset 0x8
    LIST_ENTRY Pending ; // offset 0xc
    LIST_ENTRY Complete ; // offset 0x14
    LIST_ENTRY Abort ; // offset 0x1c
    LIST_ENTRY Failed ; // offset 0x24

} POP_DEVICE_POWER_IRP, *PPOP_DEVICE_POWER_IRP, **PPPOP_DEVICE_POWER_IRP ;

typedef struct _PO_NOTIFY_ORDER_LEVEL { // 0x48 bytes
    KEVENT LevelReady ; // offset 0x0
    uint32_t DeviceCount ; // offset 0x10
    uint32_t ActiveCount ; // offset 0x14
    LIST_ENTRY WaitSleep ; // offset 0x18
    LIST_ENTRY ReadySleep ; // offset 0x20
    LIST_ENTRY Pending ; // offset 0x28
    LIST_ENTRY Complete ; // offset 0x30
    LIST_ENTRY ReadyS0 ; // offset 0x38
    LIST_ENTRY WaitS0 ; // offset 0x40

} PO_NOTIFY_ORDER_LEVEL, *PPO_NOTIFY_ORDER_LEVEL, **PPPO_NOTIFY_ORDER_LEVEL ;

typedef struct _PO_DEVICE_NOTIFY_ORDER { // 0x248 bytes
    uint32_t DevNodeSequence ; // offset 0x0
    uint32_t WarmEjectPdoPointer ; // offset 0x4
    PO_NOTIFY_ORDER_LEVEL OrderLevel[8] ; // offset 0x8

} PO_DEVICE_NOTIFY_ORDER, *PPO_DEVICE_NOTIFY_ORDER, **PPPO_DEVICE_NOTIFY_ORDER ;

typedef struct _POP_DEVICE_SYS_STATE { // 0x620 bytes
    uint8_t IrpMinor ; // offset 0x0
    SYSTEM_POWER_STATE SystemState ; // offset 0x4
    KEVENT Event ; // offset 0x8
    uint32_t SpinLock ; // offset 0x18
    uint32_t Thread ; // offset 0x1c
    uint8_t GetNewDeviceList ; // offset 0x20
    PO_DEVICE_NOTIFY_ORDER Order ; // offset 0x24
    int32_t Status ; // offset 0x26c
    uint32_t FailedDevice ; // offset 0x270
    uint8_t Waking ; // offset 0x274
    uint8_t Cancelled ; // offset 0x275
    uint8_t IgnoreErrors ; // offset 0x276
    uint8_t IgnoreNotImplemented ; // offset 0x277
    uint8_t WaitAny ; // offset 0x278
    uint8_t WaitAll ; // offset 0x279
    LIST_ENTRY PresentIrpQueue ; // offset 0x27c
    POP_DEVICE_POWER_IRP Head ; // offset 0x284
    POP_DEVICE_POWER_IRP PowerIrpState[20] ; // offset 0x2b0

} POP_DEVICE_SYS_STATE, *PPOP_DEVICE_SYS_STATE, **PPPOP_DEVICE_SYS_STATE ;

typedef struct _SEP_AUDIT_POLICY_CATEGORIES { // 0x8 bytes
    union {
    uint32_t System : 4 ; // offset 0x0
    uint32_t Logon : 4 ; // offset 0x0
    uint32_t ObjectAccess : 4 ; // offset 0x0
    uint32_t PrivilegeUse : 4 ; // offset 0x0
    uint32_t DetailedTracking : 4 ; // offset 0x0
    uint32_t PolicyChange : 4 ; // offset 0x0
    uint32_t AccountManagement : 4 ; // offset 0x0
    uint32_t DirectoryServiceAccess : 4 ; // offset 0x0
    };
    uint32_t AccountLogon : 4 ; // offset 0x4

} SEP_AUDIT_POLICY_CATEGORIES, *PSEP_AUDIT_POLICY_CATEGORIES, **PPSEP_AUDIT_POLICY_CATEGORIES ;

typedef struct _SEP_AUDIT_POLICY_OVERLAY { // 0x8 bytes
    union {
    uint64_t PolicyBits : 36 ; // offset 0x0
    uint64_t SetBit : 1 ; // offset 0x0
    };

} SEP_AUDIT_POLICY_OVERLAY, *PSEP_AUDIT_POLICY_OVERLAY, **PPSEP_AUDIT_POLICY_OVERLAY ;

typedef struct _SEP_AUDIT_POLICY { // 0x8 bytes
    union {
    SEP_AUDIT_POLICY_CATEGORIES PolicyElements ; // offset 0x0
    SEP_AUDIT_POLICY_OVERLAY PolicyOverlay ; // offset 0x0
    uint64_t Overlay ; // offset 0x0
    };

} SEP_AUDIT_POLICY, *PSEP_AUDIT_POLICY, **PPSEP_AUDIT_POLICY ;

typedef struct _TOKEN { // 0xa8 bytes
    TOKEN_SOURCE TokenSource ; // offset 0x0
    LUID TokenId ; // offset 0x10
    LUID AuthenticationId ; // offset 0x18
    LUID ParentTokenId ; // offset 0x20
    LARGE_INTEGER ExpirationTime ; // offset 0x28
    uint32_t TokenLock ; // offset 0x30
    SEP_AUDIT_POLICY AuditPolicy ; // offset 0x38
    LUID ModifiedId ; // offset 0x40
    uint32_t SessionId ; // offset 0x48
    uint32_t UserAndGroupCount ; // offset 0x4c
    uint32_t RestrictedSidCount ; // offset 0x50
    uint32_t PrivilegeCount ; // offset 0x54
    uint32_t VariableLength ; // offset 0x58
    uint32_t DynamicCharged ; // offset 0x5c
    uint32_t DynamicAvailable ; // offset 0x60
    uint32_t DefaultOwnerIndex ; // offset 0x64
    uint32_t UserAndGroups ; // offset 0x68
    uint32_t RestrictedSids ; // offset 0x6c
    uint32_t PrimaryGroup ; // offset 0x70
    uint32_t Privileges ; // offset 0x74
    uint32_t DynamicPart ; // offset 0x78
    uint32_t DefaultDacl ; // offset 0x7c
    TOKEN_TYPE TokenType ; // offset 0x80
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel ; // offset 0x84
    uint32_t TokenFlags ; // offset 0x88
    uint8_t TokenInUse ; // offset 0x8c
    uint32_t ProxyData ; // offset 0x90
    uint32_t AuditData ; // offset 0x94
    LUID OriginatingLogonSession ; // offset 0x98
    uint32_t VariablePart ; // offset 0xa0

} TOKEN, *PTOKEN, **PPTOKEN ;

typedef struct _HEAP_STOP_ON_TAG { // 0x4 bytes
    union {
    uint32_t HeapAndTagIndex ; // offset 0x0
    uint16_t TagIndex ; // offset 0x0
    };
    uint16_t HeapIndex ; // offset 0x2

} HEAP_STOP_ON_TAG, *PHEAP_STOP_ON_TAG, **PPHEAP_STOP_ON_TAG ;

typedef struct _HEAP_STOP_ON_VALUES { // 0x18 bytes
    uint32_t AllocAddress ; // offset 0x0
    HEAP_STOP_ON_TAG AllocTag ; // offset 0x4
    uint32_t ReAllocAddress ; // offset 0x8
    HEAP_STOP_ON_TAG ReAllocTag ; // offset 0xc
    uint32_t FreeAddress ; // offset 0x10
    HEAP_STOP_ON_TAG FreeTag ; // offset 0x14

} HEAP_STOP_ON_VALUES, *PHEAP_STOP_ON_VALUES, **PPHEAP_STOP_ON_VALUES ;

typedef struct _OBJECT_HEADER_NAME_INFO { // 0x10 bytes
    uint32_t Directory ; // offset 0x0
    UNICODE_STRING Name ; // offset 0x4
    uint32_t QueryReferences ; // offset 0xc

} OBJECT_HEADER_NAME_INFO, *POBJECT_HEADER_NAME_INFO, **PPOBJECT_HEADER_NAME_INFO ;

typedef struct _CHILD_LIST { // 0x8 bytes
    uint32_t Count ; // offset 0x0
    uint32_t List ; // offset 0x4

} CHILD_LIST, *PCHILD_LIST, **PPCHILD_LIST ;

typedef struct _CM_KEY_REFERENCE { // 0x8 bytes
    uint32_t KeyCell ; // offset 0x0
    uint32_t KeyHive ; // offset 0x4

} CM_KEY_REFERENCE, *PCM_KEY_REFERENCE, **PPCM_KEY_REFERENCE ;

typedef struct _CM_KEY_NODE { // 0x50 bytes
    uint16_t Signature ; // offset 0x0
    uint16_t Flags ; // offset 0x2
    LARGE_INTEGER LastWriteTime ; // offset 0x4
    uint32_t Spare ; // offset 0xc
    uint32_t Parent ; // offset 0x10
    uint32_t SubKeyCounts[2] ; // offset 0x14
    uint32_t SubKeyLists[2] ; // offset 0x1c
    CHILD_LIST ValueList ; // offset 0x24
    CM_KEY_REFERENCE ChildHiveReference ; // offset 0x1c
    uint32_t Security ; // offset 0x2c
    uint32_t Class ; // offset 0x30
    uint32_t MaxNameLen ; // offset 0x34
    uint32_t MaxClassLen ; // offset 0x38
    uint32_t MaxValueNameLen ; // offset 0x3c
    uint32_t MaxValueDataLen ; // offset 0x40
    uint32_t WorkVar ; // offset 0x44
    uint16_t NameLength ; // offset 0x48
    uint16_t ClassLength ; // offset 0x4a
    uint16_t Name[1] ; // offset 0x4c

} CM_KEY_NODE, *PCM_KEY_NODE, **PPCM_KEY_NODE ;

typedef struct _CM_KEY_VALUE { // 0x18 bytes
    uint16_t Signature ; // offset 0x0
    uint16_t NameLength ; // offset 0x2
    uint32_t DataLength ; // offset 0x4
    uint32_t Data ; // offset 0x8
    uint32_t Type ; // offset 0xc
    uint16_t Flags ; // offset 0x10
    uint16_t Spare ; // offset 0x12
    uint16_t Name[1] ; // offset 0x14

} CM_KEY_VALUE, *PCM_KEY_VALUE, **PPCM_KEY_VALUE ;

typedef struct _CM_BIG_DATA { // 0x8 bytes
    uint16_t Signature ; // offset 0x0
    uint16_t Count ; // offset 0x2
    uint32_t List ; // offset 0x4

} CM_BIG_DATA, *PCM_BIG_DATA, **PPCM_BIG_DATA ;

typedef struct _SECURITY_DESCRIPTOR_RELATIVE { // 0x14 bytes
    uint8_t Revision ; // offset 0x0
    uint8_t Sbz1 ; // offset 0x1
    uint16_t Control ; // offset 0x2
    uint32_t Owner ; // offset 0x4
    uint32_t Group ; // offset 0x8
    uint32_t Sacl ; // offset 0xc
    uint32_t Dacl ; // offset 0x10

} SECURITY_DESCRIPTOR_RELATIVE, *PSECURITY_DESCRIPTOR_RELATIVE, **PPSECURITY_DESCRIPTOR_RELATIVE ;

typedef struct _CM_KEY_SECURITY { // 0x28 bytes
    uint16_t Signature ; // offset 0x0
    uint16_t Reserved ; // offset 0x2
    uint32_t Flink ; // offset 0x4
    uint32_t Blink ; // offset 0x8
    uint32_t ReferenceCount ; // offset 0xc
    uint32_t DescriptorLength ; // offset 0x10
    SECURITY_DESCRIPTOR_RELATIVE Descriptor ; // offset 0x14

} CM_KEY_SECURITY, *PCM_KEY_SECURITY, **PPCM_KEY_SECURITY ;

typedef struct _CM_KEY_INDEX { // 0x8 bytes
    uint16_t Signature ; // offset 0x0
    uint16_t Count ; // offset 0x2
    uint32_t List[1] ; // offset 0x4

} CM_KEY_INDEX, *PCM_KEY_INDEX, **PPCM_KEY_INDEX ;

typedef union _u { // 0x50 bytes
    union {
    CM_KEY_NODE KeyNode ; // offset 0x0
    CM_KEY_VALUE KeyValue ; // offset 0x0
    CM_KEY_SECURITY KeySecurity ; // offset 0x0
    CM_KEY_INDEX KeyIndex ; // offset 0x0
    CM_BIG_DATA ValueData ; // offset 0x0
    uint32_t KeyList[1] ; // offset 0x0
    uint16_t KeyString[1] ; // offset 0x0
    };

} u, *Pu, **PPu ;

typedef struct _CELL_DATA { // 0x50 bytes
    u u ; // offset 0x0

} CELL_DATA, *PCELL_DATA, **PPCELL_DATA ;

typedef struct _CM_CACHED_VALUE_INDEX { // 0x54 bytes
    uint32_t CellIndex ; // offset 0x0
    union {
        union {
        CELL_DATA CellData ; // offset 0x0
        uint32_t List[1] ; // offset 0x0
        };
    } Data ; // offset 0x4

} CM_CACHED_VALUE_INDEX, *PCM_CACHED_VALUE_INDEX, **PPCM_CACHED_VALUE_INDEX ;

typedef struct _MMCOLOR_TABLES { // 0xc bytes
    uint32_t Flink ; // offset 0x0
    uint32_t Blink ; // offset 0x4
    uint32_t Count ; // offset 0x8

} MMCOLOR_TABLES, *PMMCOLOR_TABLES, **PPMMCOLOR_TABLES ;

typedef struct _BITMAP_RANGE { // 0x20 bytes
    LIST_ENTRY Links ; // offset 0x0
    int64_t BasePage ; // offset 0x8
    uint32_t FirstDirtyPage ; // offset 0x10
    uint32_t LastDirtyPage ; // offset 0x14
    uint32_t DirtyPages ; // offset 0x18
    uint32_t Bitmap ; // offset 0x1c

} BITMAP_RANGE, *PBITMAP_RANGE, **PPBITMAP_RANGE ;

typedef struct _MBCB { // 0x80 bytes
    int16_t NodeTypeCode ; // offset 0x0
    int16_t NodeIsInZone ; // offset 0x2
    uint32_t PagesToWrite ; // offset 0x4
    uint32_t DirtyPages ; // offset 0x8
    uint32_t Reserved ; // offset 0xc
    LIST_ENTRY BitmapRanges ; // offset 0x10
    int64_t ResumeWritePage ; // offset 0x18
    BITMAP_RANGE BitmapRange1 ; // offset 0x20
    BITMAP_RANGE BitmapRange2 ; // offset 0x40
    BITMAP_RANGE BitmapRange3 ; // offset 0x60

} MBCB, *PMBCB, **PPMBCB ;

typedef struct _KiIoAccessMap { // 0x2024 bytes
    uint8_t DirectionMap[32] ; // offset 0x0
    uint8_t IoMap[8196] ; // offset 0x20

} KiIoAccessMap, *PKiIoAccessMap, **PPKiIoAccessMap ;

typedef struct _KTSS { // 0x20ac bytes
    uint16_t Backlink ; // offset 0x0
    uint16_t Reserved0 ; // offset 0x2
    uint32_t Esp0 ; // offset 0x4
    uint16_t Ss0 ; // offset 0x8
    uint16_t Reserved1 ; // offset 0xa
    uint32_t NotUsed1[4] ; // offset 0xc
    uint32_t CR3 ; // offset 0x1c
    uint32_t Eip ; // offset 0x20
    uint32_t EFlags ; // offset 0x24
    uint32_t Eax ; // offset 0x28
    uint32_t Ecx ; // offset 0x2c
    uint32_t Edx ; // offset 0x30
    uint32_t Ebx ; // offset 0x34
    uint32_t Esp ; // offset 0x38
    uint32_t Ebp ; // offset 0x3c
    uint32_t Esi ; // offset 0x40
    uint32_t Edi ; // offset 0x44
    uint16_t Es ; // offset 0x48
    uint16_t Reserved2 ; // offset 0x4a
    uint16_t Cs ; // offset 0x4c
    uint16_t Reserved3 ; // offset 0x4e
    uint16_t Ss ; // offset 0x50
    uint16_t Reserved4 ; // offset 0x52
    uint16_t Ds ; // offset 0x54
    uint16_t Reserved5 ; // offset 0x56
    uint16_t Fs ; // offset 0x58
    uint16_t Reserved6 ; // offset 0x5a
    uint16_t Gs ; // offset 0x5c
    uint16_t Reserved7 ; // offset 0x5e
    uint16_t LDT ; // offset 0x60
    uint16_t Reserved8 ; // offset 0x62
    uint16_t Flags ; // offset 0x64
    uint16_t IoMapBase ; // offset 0x66
    KiIoAccessMap IoMaps[1] ; // offset 0x68
    uint8_t IntDirectionMap[32] ; // offset 0x208c

} KTSS, *PKTSS, **PPKTSS ;

typedef struct _PP_LOOKASIDE_LIST { // 0x8 bytes
    uint32_t P ; // offset 0x0
    uint32_t L ; // offset 0x4

} PP_LOOKASIDE_LIST, *PPP_LOOKASIDE_LIST, **PPPP_LOOKASIDE_LIST ;

typedef struct _PROCESSOR_IDLE_TIMES { // 0x20 bytes
    uint64_t StartTime ; // offset 0x0
    uint64_t EndTime ; // offset 0x8
    uint32_t IdleHandlerReserved[4] ; // offset 0x10

} PROCESSOR_IDLE_TIMES, *PPROCESSOR_IDLE_TIMES, **PPPROCESSOR_IDLE_TIMES ;

typedef struct _PROCESSOR_POWER_STATE { // 0x120 bytes
    uint32_t IdleFunction ; // offset 0x0
    uint32_t Idle0KernelTimeLimit ; // offset 0x4
    uint32_t Idle0LastTime ; // offset 0x8
    uint32_t IdleHandlers ; // offset 0xc
    uint32_t IdleState ; // offset 0x10
    uint32_t IdleHandlersCount ; // offset 0x14
    uint64_t LastCheck ; // offset 0x18
    PROCESSOR_IDLE_TIMES IdleTimes ; // offset 0x20
    uint32_t IdleTime1 ; // offset 0x40
    uint32_t PromotionCheck ; // offset 0x44
    uint32_t IdleTime2 ; // offset 0x48
    uint8_t CurrentThrottle ; // offset 0x4c
    uint8_t ThermalThrottleLimit ; // offset 0x4d
    uint8_t CurrentThrottleIndex ; // offset 0x4e
    uint8_t ThermalThrottleIndex ; // offset 0x4f
    uint32_t LastKernelUserTime ; // offset 0x50
    uint32_t LastIdleThreadKernelTime ; // offset 0x54
    uint32_t PackageIdleStartTime ; // offset 0x58
    uint32_t PackageIdleTime ; // offset 0x5c
    uint32_t DebugCount ; // offset 0x60
    uint32_t LastSysTime ; // offset 0x64
    uint64_t TotalIdleStateTime[3] ; // offset 0x68
    uint32_t TotalIdleTransitions[3] ; // offset 0x80
    uint64_t PreviousC3StateTime ; // offset 0x90
    uint8_t KneeThrottleIndex ; // offset 0x98
    uint8_t ThrottleLimitIndex ; // offset 0x99
    uint8_t PerfStatesCount ; // offset 0x9a
    uint8_t ProcessorMinThrottle ; // offset 0x9b
    uint8_t ProcessorMaxThrottle ; // offset 0x9c
    uint8_t EnableIdleAccounting ; // offset 0x9d
    uint8_t LastC3Percentage ; // offset 0x9e
    uint8_t LastAdjustedBusyPercentage ; // offset 0x9f
    uint32_t PromotionCount ; // offset 0xa0
    uint32_t DemotionCount ; // offset 0xa4
    uint32_t ErrorCount ; // offset 0xa8
    uint32_t RetryCount ; // offset 0xac
    uint32_t Flags ; // offset 0xb0
    LARGE_INTEGER PerfCounterFrequency ; // offset 0xb8
    uint32_t PerfTickCount ; // offset 0xc0
    KTIMER PerfTimer ; // offset 0xc8
    KDPC PerfDpc ; // offset 0xf0
    uint32_t PerfStates ; // offset 0x110
    uint32_t PerfSetThrottle ; // offset 0x114
    uint32_t LastC3KernelUserTime ; // offset 0x118
    uint32_t LastPackageIdleTime ; // offset 0x11c

} PROCESSOR_POWER_STATE, *PPROCESSOR_POWER_STATE, **PPPROCESSOR_POWER_STATE ;

typedef struct _FLOATING_SAVE_AREA { // 0x70 bytes
    uint32_t ControlWord ; // offset 0x0
    uint32_t StatusWord ; // offset 0x4
    uint32_t TagWord ; // offset 0x8
    uint32_t ErrorOffset ; // offset 0xc
    uint32_t ErrorSelector ; // offset 0x10
    uint32_t DataOffset ; // offset 0x14
    uint32_t DataSelector ; // offset 0x18
    uint8_t RegisterArea[80] ; // offset 0x1c
    uint32_t Cr0NpxState ; // offset 0x6c

} FLOATING_SAVE_AREA, *PFLOATING_SAVE_AREA, **PPFLOATING_SAVE_AREA ;

typedef struct _CONTEXT { // 0x2cc bytes
    uint32_t ContextFlags ; // offset 0x0
    uint32_t Dr0 ; // offset 0x4
    uint32_t Dr1 ; // offset 0x8
    uint32_t Dr2 ; // offset 0xc
    uint32_t Dr3 ; // offset 0x10
    uint32_t Dr6 ; // offset 0x14
    uint32_t Dr7 ; // offset 0x18
    FLOATING_SAVE_AREA FloatSave ; // offset 0x1c
    uint32_t SegGs ; // offset 0x8c
    uint32_t SegFs ; // offset 0x90
    uint32_t SegEs ; // offset 0x94
    uint32_t SegDs ; // offset 0x98
    uint32_t Edi ; // offset 0x9c
    uint32_t Esi ; // offset 0xa0
    uint32_t Ebx ; // offset 0xa4
    uint32_t Edx ; // offset 0xa8
    uint32_t Ecx ; // offset 0xac
    uint32_t Eax ; // offset 0xb0
    uint32_t Ebp ; // offset 0xb4
    uint32_t Eip ; // offset 0xb8
    uint32_t SegCs ; // offset 0xbc
    uint32_t EFlags ; // offset 0xc0
    uint32_t Esp ; // offset 0xc4
    uint32_t SegSs ; // offset 0xc8
    uint8_t ExtendedRegisters[512] ; // offset 0xcc

} CONTEXT, *PCONTEXT, **PPCONTEXT ;

typedef struct _DESCRIPTOR { // 0x8 bytes
    uint16_t Pad ; // offset 0x0
    uint16_t Limit ; // offset 0x2
    uint32_t Base ; // offset 0x4

} DESCRIPTOR, *PDESCRIPTOR, **PPDESCRIPTOR ;

typedef struct _KSPECIAL_REGISTERS { // 0x54 bytes
    uint32_t Cr0 ; // offset 0x0
    uint32_t Cr2 ; // offset 0x4
    uint32_t Cr3 ; // offset 0x8
    uint32_t Cr4 ; // offset 0xc
    uint32_t KernelDr0 ; // offset 0x10
    uint32_t KernelDr1 ; // offset 0x14
    uint32_t KernelDr2 ; // offset 0x18
    uint32_t KernelDr3 ; // offset 0x1c
    uint32_t KernelDr6 ; // offset 0x20
    uint32_t KernelDr7 ; // offset 0x24
    DESCRIPTOR Gdtr ; // offset 0x28
    DESCRIPTOR Idtr ; // offset 0x30
    uint16_t Tr ; // offset 0x38
    uint16_t Ldtr ; // offset 0x3a
    uint32_t Reserved[6] ; // offset 0x3c

} KSPECIAL_REGISTERS, *PKSPECIAL_REGISTERS, **PPKSPECIAL_REGISTERS ;

typedef struct _KPROCESSOR_STATE { // 0x320 bytes
    CONTEXT ContextFrame ; // offset 0x0
    KSPECIAL_REGISTERS SpecialRegisters ; // offset 0x2cc

} KPROCESSOR_STATE, *PKPROCESSOR_STATE, **PPKPROCESSOR_STATE ;

typedef struct _FNSAVE_FORMAT { // 0x6c bytes
    uint32_t ControlWord ; // offset 0x0
    uint32_t StatusWord ; // offset 0x4
    uint32_t TagWord ; // offset 0x8
    uint32_t ErrorOffset ; // offset 0xc
    uint32_t ErrorSelector ; // offset 0x10
    uint32_t DataOffset ; // offset 0x14
    uint32_t DataSelector ; // offset 0x18
    uint8_t RegisterArea[80] ; // offset 0x1c

} FNSAVE_FORMAT, *PFNSAVE_FORMAT, **PPFNSAVE_FORMAT ;

typedef struct _FXSAVE_FORMAT { // 0x208 bytes
    uint16_t ControlWord ; // offset 0x0
    uint16_t StatusWord ; // offset 0x2
    uint16_t TagWord ; // offset 0x4
    uint16_t ErrorOpcode ; // offset 0x6
    uint32_t ErrorOffset ; // offset 0x8
    uint32_t ErrorSelector ; // offset 0xc
    uint32_t DataOffset ; // offset 0x10
    uint32_t DataSelector ; // offset 0x14
    uint32_t MXCsr ; // offset 0x18
    uint32_t MXCsrMask ; // offset 0x1c
    uint8_t RegisterArea[128] ; // offset 0x20
    uint8_t Reserved3[128] ; // offset 0xa0
    uint8_t Reserved4[224] ; // offset 0x120
    uint8_t Align16Byte[8] ; // offset 0x200

} FXSAVE_FORMAT, *PFXSAVE_FORMAT, **PPFXSAVE_FORMAT ;

typedef struct _FX_SAVE_AREA { // 0x210 bytes
    union {
        union {
        FNSAVE_FORMAT FnArea ; // offset 0x0
        FXSAVE_FORMAT FxArea ; // offset 0x0
        };
    } U ; // offset 0x0
    uint32_t NpxSavedCpu ; // offset 0x208
    uint32_t Cr0NpxState ; // offset 0x20c

} FX_SAVE_AREA, *PFX_SAVE_AREA, **PPFX_SAVE_AREA ;

typedef struct _KPRCB { // 0xc50 bytes
    uint16_t MinorVersion ; // offset 0x0
    uint16_t MajorVersion ; // offset 0x2
    uint32_t CurrentThread ; // offset 0x4
    uint32_t NextThread ; // offset 0x8
    uint32_t IdleThread ; // offset 0xc
    uint8_t Number ; // offset 0x10
    uint8_t Reserved ; // offset 0x11
    uint16_t BuildType ; // offset 0x12
    uint32_t SetMember ; // offset 0x14
    uint8_t CpuType ; // offset 0x18
    uint8_t CpuID ; // offset 0x19
    uint16_t CpuStep ; // offset 0x1a
    KPROCESSOR_STATE ProcessorState ; // offset 0x1c
    uint32_t KernelReserved[16] ; // offset 0x33c
    uint32_t HalReserved[16] ; // offset 0x37c
    uint8_t PrcbPad0[92] ; // offset 0x3bc
    KSPIN_LOCK_QUEUE LockQueue[16] ; // offset 0x418
    uint8_t PrcbPad1[8] ; // offset 0x498
    uint32_t NpxThread ; // offset 0x4a0
    uint32_t InterruptCount ; // offset 0x4a4
    uint32_t KernelTime ; // offset 0x4a8
    uint32_t UserTime ; // offset 0x4ac
    uint32_t DpcTime ; // offset 0x4b0
    uint32_t DebugDpcTime ; // offset 0x4b4
    uint32_t InterruptTime ; // offset 0x4b8
    uint32_t AdjustDpcThreshold ; // offset 0x4bc
    uint32_t PageColor ; // offset 0x4c0
    uint32_t SkipTick ; // offset 0x4c4
    uint8_t MultiThreadSetBusy ; // offset 0x4c8
    uint8_t Spare2[3] ; // offset 0x4c9
    uint32_t ParentNode ; // offset 0x4cc
    uint32_t MultiThreadProcessorSet ; // offset 0x4d0
    uint32_t MultiThreadSetMaster ; // offset 0x4d4
    uint32_t ThreadStartCount[2] ; // offset 0x4d8
    uint32_t CcFastReadNoWait ; // offset 0x4e0
    uint32_t CcFastReadWait ; // offset 0x4e4
    uint32_t CcFastReadNotPossible ; // offset 0x4e8
    uint32_t CcCopyReadNoWait ; // offset 0x4ec
    uint32_t CcCopyReadWait ; // offset 0x4f0
    uint32_t CcCopyReadNoWaitMiss ; // offset 0x4f4
    uint32_t KeAlignmentFixupCount ; // offset 0x4f8
    uint32_t KeContextSwitches ; // offset 0x4fc
    uint32_t KeDcacheFlushCount ; // offset 0x500
    uint32_t KeExceptionDispatchCount ; // offset 0x504
    uint32_t KeFirstLevelTbFills ; // offset 0x508
    uint32_t KeFloatingEmulationCount ; // offset 0x50c
    uint32_t KeIcacheFlushCount ; // offset 0x510
    uint32_t KeSecondLevelTbFills ; // offset 0x514
    uint32_t KeSystemCalls ; // offset 0x518
    uint32_t SpareCounter0[1] ; // offset 0x51c
    PP_LOOKASIDE_LIST PPLookasideList[16] ; // offset 0x520
    PP_LOOKASIDE_LIST PPNPagedLookasideList[32] ; // offset 0x5a0
    PP_LOOKASIDE_LIST PPPagedLookasideList[32] ; // offset 0x6a0
    volatile uint32_t PacketBarrier ; // offset 0x7a0
    volatile uint32_t ReverseStall ; // offset 0x7a4
    uint32_t IpiFrame ; // offset 0x7a8
    uint8_t PrcbPad2[52] ; // offset 0x7ac
    uint32_t CurrentPacket[3] ; // offset 0x7e0
    volatile uint32_t TargetSet ; // offset 0x7ec
    uint32_t WorkerRoutine ; // offset 0x7f0
    volatile uint32_t IpiFrozen ; // offset 0x7f4
    uint8_t PrcbPad3[40] ; // offset 0x7f8
    volatile uint32_t RequestSummary ; // offset 0x820
    uint32_t SignalDone ; // offset 0x824
    uint8_t PrcbPad4[56] ; // offset 0x828
    LIST_ENTRY DpcListHead ; // offset 0x860
    uint32_t DpcStack ; // offset 0x868
    uint32_t DpcCount ; // offset 0x86c
    volatile uint32_t DpcQueueDepth ; // offset 0x870
    volatile uint32_t DpcRoutineActive ; // offset 0x874
    volatile uint32_t DpcInterruptRequested ; // offset 0x878
    uint32_t DpcLastCount ; // offset 0x87c
    uint32_t DpcRequestRate ; // offset 0x880
    uint32_t MaximumDpcQueueDepth ; // offset 0x884
    uint32_t MinimumDpcRate ; // offset 0x888
    uint32_t QuantumEnd ; // offset 0x88c
    uint8_t PrcbPad5[16] ; // offset 0x890
    uint32_t DpcLock ; // offset 0x8a0
    uint8_t PrcbPad6[28] ; // offset 0x8a4
    KDPC CallDpc ; // offset 0x8c0
    uint32_t ChainedInterruptList ; // offset 0x8e0
    int32_t LookasideIrpFloat ; // offset 0x8e4
    uint32_t SpareFields0[6] ; // offset 0x8e8
    uint8_t VendorString[13] ; // offset 0x900
    uint8_t InitialApicId ; // offset 0x90d
    uint8_t LogicalProcessorsPerPhysicalProcessor ; // offset 0x90e
    uint32_t MHz ; // offset 0x910
    uint32_t FeatureBits ; // offset 0x914
    LARGE_INTEGER UpdateSignature ; // offset 0x918
    FX_SAVE_AREA NpxSaveArea ; // offset 0x920
    PROCESSOR_POWER_STATE PowerState ; // offset 0xb30

} KPRCB, *PKPRCB, **PPKPRCB ;

typedef struct _KPCR { // 0xd70 bytes
    NT_TIB NtTib ; // offset 0x0
    uint32_t SelfPcr ; // offset 0x1c
    uint32_t Prcb ; // offset 0x20
    uint8_t Irql ; // offset 0x24
    uint32_t IRR ; // offset 0x28
    uint32_t IrrActive ; // offset 0x2c
    uint32_t IDR ; // offset 0x30
    uint32_t KdVersionBlock ; // offset 0x34
    uint32_t IDT ; // offset 0x38
    uint32_t GDT ; // offset 0x3c
    uint32_t TSS ; // offset 0x40
    uint16_t MajorVersion ; // offset 0x44
    uint16_t MinorVersion ; // offset 0x46
    uint32_t SetMember ; // offset 0x48
    uint32_t StallScaleFactor ; // offset 0x4c
    uint8_t DebugActive ; // offset 0x50
    uint8_t Number ; // offset 0x51
    uint8_t Spare0 ; // offset 0x52
    uint8_t SecondLevelCacheAssociativity ; // offset 0x53
    uint32_t VdmAlert ; // offset 0x54
    uint32_t KernelReserved[14] ; // offset 0x58
    uint32_t SecondLevelCacheSize ; // offset 0x90
    uint32_t HalReserved[16] ; // offset 0x94
    uint32_t InterruptMode ; // offset 0xd4
    uint8_t Spare1 ; // offset 0xd8
    uint32_t KernelReserved2[17] ; // offset 0xdc
    KPRCB PrcbData ; // offset 0x120

} KPCR, *PKPCR, **PPKPCR ;

typedef struct _CALL_HASH_ENTRY { // 0x14 bytes
    LIST_ENTRY ListEntry ; // offset 0x0
    uint32_t CallersAddress ; // offset 0x8
    uint32_t CallersCaller ; // offset 0xc
    uint32_t CallCount ; // offset 0x10

} CALL_HASH_ENTRY, *PCALL_HASH_ENTRY, **PPCALL_HASH_ENTRY ;

typedef struct _MMPFNLIST { // 0x10 bytes
    uint32_t Total ; // offset 0x0
    MMLISTS ListName ; // offset 0x4
    uint32_t Flink ; // offset 0x8
    uint32_t Blink ; // offset 0xc

} MMPFNLIST, *PMMPFNLIST, **PPMMPFNLIST ;

typedef struct _EX_PUSH_LOCK_CACHE_AWARE { // 0x4 bytes
    uint32_t Locks[1] ; // offset 0x0

} EX_PUSH_LOCK_CACHE_AWARE, *PEX_PUSH_LOCK_CACHE_AWARE, **PPEX_PUSH_LOCK_CACHE_AWARE ;

typedef struct _PLUGPLAY_EVENT_BLOCK { // 0x38 bytes
    GUID EventGuid ; // offset 0x0
    PLUGPLAY_EVENT_CATEGORY EventCategory ; // offset 0x10
    uint32_t Result ; // offset 0x14
    uint32_t Flags ; // offset 0x18
    uint32_t TotalSize ; // offset 0x1c
    uint32_t DeviceObject ; // offset 0x20
    union {
        union {
        struct {
                    GUID ClassGuid ; // offset 0x0
                    uint16_t SymbolicLinkName[1] ; // offset 0x10
                } DeviceClass ; // offset 0x0
        struct {
                    uint16_t DeviceIds[1] ; // offset 0x0
                } TargetDevice ; // offset 0x0
        struct {
                    uint16_t DeviceId[1] ; // offset 0x0
                } InstallDevice ; // offset 0x0
        struct {
                    uint32_t NotificationStructure ; // offset 0x0
                    uint16_t DeviceIds[1] ; // offset 0x4
                } CustomNotification ; // offset 0x0
        struct {
                    uint32_t Notification ; // offset 0x0
                } ProfileNotification ; // offset 0x0
        struct {
                    uint32_t NotificationCode ; // offset 0x0
                    uint32_t NotificationData ; // offset 0x4
                } PowerNotification ; // offset 0x0
        struct {
                    PNP_VETO_TYPE VetoType ; // offset 0x0
                    uint16_t DeviceIdVetoNameBuffer[1] ; // offset 0x4
                } VetoNotification ; // offset 0x0
        struct {
                    GUID BlockedDriverGuid ; // offset 0x0
                } BlockedDriverNotification ; // offset 0x0
        };
    } u ; // offset 0x24

} PLUGPLAY_EVENT_BLOCK, *PPLUGPLAY_EVENT_BLOCK, **PPPLUGPLAY_EVENT_BLOCK ;

typedef struct _PNP_DEVICE_EVENT_ENTRY { // 0x58 bytes
    LIST_ENTRY ListEntry ; // offset 0x0
    uint32_t Argument ; // offset 0x8
    uint32_t CallerEvent ; // offset 0xc
    uint32_t Callback ; // offset 0x10
    uint32_t Context ; // offset 0x14
    uint32_t VetoType ; // offset 0x18
    uint32_t VetoName ; // offset 0x1c
    PLUGPLAY_EVENT_BLOCK Data ; // offset 0x20

} PNP_DEVICE_EVENT_ENTRY, *PPNP_DEVICE_EVENT_ENTRY, **PPPNP_DEVICE_EVENT_ENTRY ;

typedef struct _EXCEPTION_RECORD64 { // 0x98 bytes
    int32_t ExceptionCode ; // offset 0x0
    uint32_t ExceptionFlags ; // offset 0x4
    uint64_t ExceptionRecord ; // offset 0x8
    uint64_t ExceptionAddress ; // offset 0x10
    uint32_t NumberParameters ; // offset 0x18
    uint32_t __unusedAlignment ; // offset 0x1c
    uint64_t ExceptionInformation[15] ; // offset 0x20

} EXCEPTION_RECORD64, *PEXCEPTION_RECORD64, **PPEXCEPTION_RECORD64 ;

typedef struct _DBGKM_EXCEPTION64 { // 0xa0 bytes
    EXCEPTION_RECORD64 ExceptionRecord ; // offset 0x0
    uint32_t FirstChance ; // offset 0x98

} DBGKM_EXCEPTION64, *PDBGKM_EXCEPTION64, **PPDBGKM_EXCEPTION64 ;

typedef struct _CACHE_MANAGER_CALLBACKS { // 0x10 bytes
    uint32_t AcquireForLazyWrite ; // offset 0x0
    uint32_t ReleaseFromLazyWrite ; // offset 0x4
    uint32_t AcquireForReadAhead ; // offset 0x8
    uint32_t ReleaseFromReadAhead ; // offset 0xc

} CACHE_MANAGER_CALLBACKS, *PCACHE_MANAGER_CALLBACKS, **PPCACHE_MANAGER_CALLBACKS ;

typedef struct _PCI_SECONDARY_EXTENSION { // 0xc bytes
    SINGLE_LIST_ENTRY List ; // offset 0x0
    PCI_SIGNATURE ExtensionType ; // offset 0x4
    uint32_t Destructor ; // offset 0x8

} PCI_SECONDARY_EXTENSION, *PPCI_SECONDARY_EXTENSION, **PPPCI_SECONDARY_EXTENSION ;

typedef struct _ARBITER_ORDERING_LIST { // 0x8 bytes
    uint16_t Count ; // offset 0x0
    uint16_t Maximum ; // offset 0x2
    uint32_t Orderings ; // offset 0x4

} ARBITER_ORDERING_LIST, *PARBITER_ORDERING_LIST, **PPARBITER_ORDERING_LIST ;

typedef struct _ARBITER_INSTANCE { // 0x9c bytes
    uint32_t Signature ; // offset 0x0
    uint32_t MutexEvent ; // offset 0x4
    uint32_t Name ; // offset 0x8
    int32_t ResourceType ; // offset 0xc
    uint32_t Allocation ; // offset 0x10
    uint32_t PossibleAllocation ; // offset 0x14
    ARBITER_ORDERING_LIST OrderingList ; // offset 0x18
    ARBITER_ORDERING_LIST ReservedList ; // offset 0x20
    int32_t ReferenceCount ; // offset 0x28
    uint32_t Interface ; // offset 0x2c
    uint32_t AllocationStackMaxSize ; // offset 0x30
    uint32_t AllocationStack ; // offset 0x34
    uint32_t UnpackRequirement ; // offset 0x38
    uint32_t PackResource ; // offset 0x3c
    uint32_t UnpackResource ; // offset 0x40
    uint32_t ScoreRequirement ; // offset 0x44
    uint32_t TestAllocation ; // offset 0x48
    uint32_t RetestAllocation ; // offset 0x4c
    uint32_t CommitAllocation ; // offset 0x50
    uint32_t RollbackAllocation ; // offset 0x54
    uint32_t BootAllocation ; // offset 0x58
    uint32_t QueryArbitrate ; // offset 0x5c
    uint32_t QueryConflict ; // offset 0x60
    uint32_t AddReserved ; // offset 0x64
    uint32_t StartArbiter ; // offset 0x68
    uint32_t PreprocessEntry ; // offset 0x6c
    uint32_t AllocateEntry ; // offset 0x70
    uint32_t GetNextAllocationRange ; // offset 0x74
    uint32_t FindSuitableRange ; // offset 0x78
    uint32_t AddAllocation ; // offset 0x7c
    uint32_t BacktrackAllocation ; // offset 0x80
    uint32_t OverrideConflict ; // offset 0x84
    uint8_t TransactionInProgress ; // offset 0x88
    uint32_t Extension ; // offset 0x8c
    uint32_t BusDeviceObject ; // offset 0x90
    uint32_t ConflictCallbackContext ; // offset 0x94
    uint32_t ConflictCallback ; // offset 0x98

} ARBITER_INSTANCE, *PARBITER_INSTANCE, **PPARBITER_INSTANCE ;

typedef struct _PCI_ARBITER_INSTANCE { // 0xe0 bytes
    PCI_SECONDARY_EXTENSION Header ; // offset 0x0
    uint32_t Interface ; // offset 0xc
    uint32_t BusFdoExtension ; // offset 0x10
    uint16_t InstanceName[24] ; // offset 0x14
    ARBITER_INSTANCE CommonInstance ; // offset 0x44

} PCI_ARBITER_INSTANCE, *PPCI_ARBITER_INSTANCE, **PPPCI_ARBITER_INSTANCE ;

typedef struct _POP_IDLE_HANDLER { // 0x20 bytes
    uint32_t Latency ; // offset 0x0
    uint32_t TimeCheck ; // offset 0x4
    uint32_t DemoteLimit ; // offset 0x8
    uint32_t PromoteLimit ; // offset 0xc
    uint32_t PromoteCount ; // offset 0x10
    uint8_t Demote ; // offset 0x14
    uint8_t Promote ; // offset 0x15
    uint8_t PromotePercent ; // offset 0x16
    uint8_t DemotePercent ; // offset 0x17
    uint8_t State ; // offset 0x18
    uint8_t Spare[3] ; // offset 0x19
    uint32_t IdleFunction ; // offset 0x1c

} POP_IDLE_HANDLER, *PPOP_IDLE_HANDLER, **PPPOP_IDLE_HANDLER ;

typedef struct _POP_TRIGGER_WAIT { // 0x20 bytes
    KEVENT Event ; // offset 0x0
    int32_t Status ; // offset 0x10
    LIST_ENTRY Link ; // offset 0x14
    uint32_t Trigger ; // offset 0x1c

} POP_TRIGGER_WAIT, *PPOP_TRIGGER_WAIT, **PPPOP_TRIGGER_WAIT ;

typedef struct _ARBITER_ORDERING { // 0x10 bytes
    uint64_t Start ; // offset 0x0
    uint64_t End ; // offset 0x8

} ARBITER_ORDERING, *PARBITER_ORDERING, **PPARBITER_ORDERING ;

typedef struct _OBJECT_NAME_INFORMATION { // 0x8 bytes
    UNICODE_STRING Name ; // offset 0x0

} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION, **PPOBJECT_NAME_INFORMATION ;

typedef struct _VACB_LEVEL_REFERENCE { // 0x8 bytes
    int32_t Reference ; // offset 0x0
    int32_t SpecialReference ; // offset 0x4

} VACB_LEVEL_REFERENCE, *PVACB_LEVEL_REFERENCE, **PPVACB_LEVEL_REFERENCE ;

typedef struct _flags { // 0x1 bytes
    uint8_t Removable ; // offset 0x0

} flags, *Pflags, **PPflags ;

typedef struct _KNODE { // 0x30 bytes
    uint32_t ProcessorMask ; // offset 0x0
    uint32_t Color ; // offset 0x4
    uint32_t MmShiftedColor ; // offset 0x8
    uint32_t FreeCount[2] ; // offset 0xc
    SLIST_HEADER DeadStackList ; // offset 0x18
    SLIST_HEADER PfnDereferenceSListHead ; // offset 0x20
    uint32_t PfnDeferredList ; // offset 0x28
    uint8_t Seed ; // offset 0x2c
    flags Flags ; // offset 0x2d

} KNODE, *PKNODE, **PPKNODE ;

typedef struct _PCI_HEADER_TYPE_0 { // 0x30 bytes
    uint32_t BaseAddresses[6] ; // offset 0x0
    uint32_t CIS ; // offset 0x18
    uint16_t SubVendorID ; // offset 0x1c
    uint16_t SubSystemID ; // offset 0x1e
    uint32_t ROMBaseAddress ; // offset 0x20
    uint8_t CapabilitiesPtr ; // offset 0x24
    uint8_t Reserved1[3] ; // offset 0x25
    uint32_t Reserved2 ; // offset 0x28
    uint8_t InterruptLine ; // offset 0x2c
    uint8_t InterruptPin ; // offset 0x2d
    uint8_t MinimumGrant ; // offset 0x2e
    uint8_t MaximumLatency ; // offset 0x2f

} PCI_HEADER_TYPE_0, *PPCI_HEADER_TYPE_0, **PPPCI_HEADER_TYPE_0 ;

typedef struct _PCI_HEADER_TYPE_1 { // 0x30 bytes
    uint32_t BaseAddresses[2] ; // offset 0x0
    uint8_t PrimaryBus ; // offset 0x8
    uint8_t SecondaryBus ; // offset 0x9
    uint8_t SubordinateBus ; // offset 0xa
    uint8_t SecondaryLatency ; // offset 0xb
    uint8_t IOBase ; // offset 0xc
    uint8_t IOLimit ; // offset 0xd
    uint16_t SecondaryStatus ; // offset 0xe
    uint16_t MemoryBase ; // offset 0x10
    uint16_t MemoryLimit ; // offset 0x12
    uint16_t PrefetchBase ; // offset 0x14
    uint16_t PrefetchLimit ; // offset 0x16
    uint32_t PrefetchBaseUpper32 ; // offset 0x18
    uint32_t PrefetchLimitUpper32 ; // offset 0x1c
    uint16_t IOBaseUpper16 ; // offset 0x20
    uint16_t IOLimitUpper16 ; // offset 0x22
    uint8_t CapabilitiesPtr ; // offset 0x24
    uint8_t Reserved1[3] ; // offset 0x25
    uint32_t ROMBaseAddress ; // offset 0x28
    uint8_t InterruptLine ; // offset 0x2c
    uint8_t InterruptPin ; // offset 0x2d
    uint16_t BridgeControl ; // offset 0x2e

} PCI_HEADER_TYPE_1, *PPCI_HEADER_TYPE_1, **PPPCI_HEADER_TYPE_1 ;

typedef struct _PCI_HEADER_TYPE_2 { // 0x30 bytes
    uint32_t SocketRegistersBaseAddress ; // offset 0x0
    uint8_t CapabilitiesPtr ; // offset 0x4
    uint8_t Reserved ; // offset 0x5
    uint16_t SecondaryStatus ; // offset 0x6
    uint8_t PrimaryBus ; // offset 0x8
    uint8_t SecondaryBus ; // offset 0x9
    uint8_t SubordinateBus ; // offset 0xa
    uint8_t SecondaryLatency ; // offset 0xb
    struct {
        uint32_t Base ; // offset 0x0
        uint32_t Limit ; // offset 0x4
    } Range[4] ; // offset 0xc
    uint8_t InterruptLine ; // offset 0x2c
    uint8_t InterruptPin ; // offset 0x2d
    uint16_t BridgeControl ; // offset 0x2e

} PCI_HEADER_TYPE_2, *PPCI_HEADER_TYPE_2, **PPPCI_HEADER_TYPE_2 ;

typedef struct _PCI_COMMON_CONFIG { // 0x100 bytes
    uint16_t VendorID ; // offset 0x0
    uint16_t DeviceID ; // offset 0x2
    uint16_t Command ; // offset 0x4
    uint16_t Status ; // offset 0x6
    uint8_t RevisionID ; // offset 0x8
    uint8_t ProgIf ; // offset 0x9
    uint8_t SubClass ; // offset 0xa
    uint8_t BaseClass ; // offset 0xb
    uint8_t CacheLineSize ; // offset 0xc
    uint8_t LatencyTimer ; // offset 0xd
    uint8_t HeaderType ; // offset 0xe
    uint8_t BIST ; // offset 0xf
    union {
        union {
        PCI_HEADER_TYPE_0 type0 ; // offset 0x0
        PCI_HEADER_TYPE_1 type1 ; // offset 0x0
        PCI_HEADER_TYPE_2 type2 ; // offset 0x0
        };
    } u ; // offset 0x10
    uint8_t DeviceSpecific[192] ; // offset 0x40

} PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG, **PPPCI_COMMON_CONFIG ;

typedef struct _SEGMENT { // 0x40 bytes
    uint32_t ControlArea ; // offset 0x0
    uint32_t TotalNumberOfPtes ; // offset 0x4
    uint32_t NonExtendedPtes ; // offset 0x8
    uint32_t WritableUserReferences ; // offset 0xc
    uint64_t SizeOfSegment ; // offset 0x10
    MMPTE SegmentPteTemplate ; // offset 0x18
    uint32_t NumberOfCommittedPages ; // offset 0x1c
    uint32_t ExtendInfo ; // offset 0x20
    uint32_t SystemImageBase ; // offset 0x24
    uint32_t BasedAddress ; // offset 0x28
    union {
        union {
        uint32_t ImageCommitment ; // offset 0x0
        uint32_t CreatingProcess ; // offset 0x0
        };
    } u1 ; // offset 0x2c
    union {
        union {
        uint32_t ImageInformation ; // offset 0x0
        uint32_t FirstMappedVa ; // offset 0x0
        };
    } u2 ; // offset 0x30
    uint32_t PrototypePte ; // offset 0x34
    MMPTE ThePtes[1] ; // offset 0x38

} SEGMENT, *PSEGMENT, **PPSEGMENT ;

typedef struct _SECURITY_TOKEN_PROXY_DATA { // 0x18 bytes
    uint32_t Length ; // offset 0x0
    PROXY_CLASS ProxyClass ; // offset 0x4
    UNICODE_STRING PathInfo ; // offset 0x8
    uint32_t ContainerMask ; // offset 0x10
    uint32_t ObjectMask ; // offset 0x14

} SECURITY_TOKEN_PROXY_DATA, *PSECURITY_TOKEN_PROXY_DATA, **PPSECURITY_TOKEN_PROXY_DATA ;

typedef struct _HEAP_LOCK { // 0x38 bytes
    union {
        union {
        RTL_CRITICAL_SECTION CriticalSection ; // offset 0x0
        ERESOURCE Resource ; // offset 0x0
        };
    } Lock ; // offset 0x0

} HEAP_LOCK, *PHEAP_LOCK, **PPHEAP_LOCK ;

typedef struct _MMMOD_WRITER_LISTHEAD { // 0x18 bytes
    LIST_ENTRY ListHead ; // offset 0x0
    KEVENT Event ; // offset 0x8

} MMMOD_WRITER_LISTHEAD, *PMMMOD_WRITER_LISTHEAD, **PPMMMOD_WRITER_LISTHEAD ;

typedef struct _POWER_CHANNEL_SUMMARY { // 0x14 bytes
    uint32_t Signature ; // offset 0x0
    uint32_t TotalCount ; // offset 0x4
    uint32_t D0Count ; // offset 0x8
    LIST_ENTRY NotifyList ; // offset 0xc

} POWER_CHANNEL_SUMMARY, *PPOWER_CHANNEL_SUMMARY, **PPPOWER_CHANNEL_SUMMARY ;

typedef struct _DEVICE_OBJECT_POWER_EXTENSION { // 0x4c bytes
    uint32_t IdleCount ; // offset 0x0
    uint32_t ConservationIdleTime ; // offset 0x4
    uint32_t PerformanceIdleTime ; // offset 0x8
    uint32_t DeviceObject ; // offset 0xc
    LIST_ENTRY IdleList ; // offset 0x10
    uint8_t DeviceType ; // offset 0x18
    DEVICE_POWER_STATE State ; // offset 0x1c
    LIST_ENTRY NotifySourceList ; // offset 0x20
    LIST_ENTRY NotifyTargetList ; // offset 0x28
    POWER_CHANNEL_SUMMARY PowerChannelSummary ; // offset 0x30
    LIST_ENTRY Volume ; // offset 0x44

} DEVICE_OBJECT_POWER_EXTENSION, *PDEVICE_OBJECT_POWER_EXTENSION, **PPDEVICE_OBJECT_POWER_EXTENSION ;

typedef struct _DBGKD_READ_MEMORY32 { // 0xc bytes
    uint32_t TargetBaseAddress ; // offset 0x0
    uint32_t TransferCount ; // offset 0x4
    uint32_t ActualBytesRead ; // offset 0x8

} DBGKD_READ_MEMORY32, *PDBGKD_READ_MEMORY32, **PPDBGKD_READ_MEMORY32 ;

typedef struct _DBGKD_SET_SPECIAL_CALL32 { // 0x4 bytes
    uint32_t SpecialCall ; // offset 0x0

} DBGKD_SET_SPECIAL_CALL32, *PDBGKD_SET_SPECIAL_CALL32, **PPDBGKD_SET_SPECIAL_CALL32 ;

typedef struct _DBGKD_GET_VERSION32 { // 0x28 bytes
    uint16_t MajorVersion ; // offset 0x0
    uint16_t MinorVersion ; // offset 0x2
    uint16_t ProtocolVersion ; // offset 0x4
    uint16_t Flags ; // offset 0x6
    uint32_t KernBase ; // offset 0x8
    uint32_t PsLoadedModuleList ; // offset 0xc
    uint16_t MachineType ; // offset 0x10
    uint16_t ThCallbackStack ; // offset 0x12
    uint16_t NextCallback ; // offset 0x14
    uint16_t FramePointer ; // offset 0x16
    uint32_t KiCallUserMode ; // offset 0x18
    uint32_t KeUserCallbackDispatcher ; // offset 0x1c
    uint32_t BreakpointWithStatus ; // offset 0x20
    uint32_t DebuggerDataList ; // offset 0x24

} DBGKD_GET_VERSION32, *PDBGKD_GET_VERSION32, **PPDBGKD_GET_VERSION32 ;

typedef struct _DBGKD_GET_INTERNAL_BREAKPOINT32 { // 0x1c bytes
    uint32_t BreakpointAddress ; // offset 0x0
    uint32_t Flags ; // offset 0x4
    uint32_t Calls ; // offset 0x8
    uint32_t MaxCallsPerPeriod ; // offset 0xc
    uint32_t MinInstructions ; // offset 0x10
    uint32_t MaxInstructions ; // offset 0x14
    uint32_t TotalInstructions ; // offset 0x18

} DBGKD_GET_INTERNAL_BREAKPOINT32, *PDBGKD_GET_INTERNAL_BREAKPOINT32, **PPDBGKD_GET_INTERNAL_BREAKPOINT32 ;

typedef struct _DBGKD_WRITE_MEMORY32 { // 0xc bytes
    uint32_t TargetBaseAddress ; // offset 0x0
    uint32_t TransferCount ; // offset 0x4
    uint32_t ActualBytesWritten ; // offset 0x8

} DBGKD_WRITE_MEMORY32, *PDBGKD_WRITE_MEMORY32, **PPDBGKD_WRITE_MEMORY32 ;

typedef struct _DBGKD_SET_INTERNAL_BREAKPOINT32 { // 0x8 bytes
    uint32_t BreakpointAddress ; // offset 0x0
    uint32_t Flags ; // offset 0x4

} DBGKD_SET_INTERNAL_BREAKPOINT32, *PDBGKD_SET_INTERNAL_BREAKPOINT32, **PPDBGKD_SET_INTERNAL_BREAKPOINT32 ;

typedef struct _DBGKD_READ_WRITE_IO32 { // 0xc bytes
    uint32_t DataSize ; // offset 0x0
    uint32_t IoAddress ; // offset 0x4
    uint32_t DataValue ; // offset 0x8

} DBGKD_READ_WRITE_IO32, *PDBGKD_READ_WRITE_IO32, **PPDBGKD_READ_WRITE_IO32 ;

typedef struct _DBGKD_READ_WRITE_IO_EXTENDED32 { // 0x18 bytes
    uint32_t DataSize ; // offset 0x0
    uint32_t InterfaceType ; // offset 0x4
    uint32_t BusNumber ; // offset 0x8
    uint32_t AddressSpace ; // offset 0xc
    uint32_t IoAddress ; // offset 0x10
    uint32_t DataValue ; // offset 0x14

} DBGKD_READ_WRITE_IO_EXTENDED32, *PDBGKD_READ_WRITE_IO_EXTENDED32, **PPDBGKD_READ_WRITE_IO_EXTENDED32 ;

typedef struct _DBGKD_WRITE_BREAKPOINT32 { // 0x8 bytes
    uint32_t BreakPointAddress ; // offset 0x0
    uint32_t BreakPointHandle ; // offset 0x4

} DBGKD_WRITE_BREAKPOINT32, *PDBGKD_WRITE_BREAKPOINT32, **PPDBGKD_WRITE_BREAKPOINT32 ;

typedef struct _DBGKD_MANIPULATE_STATE32 { // 0x34 bytes
    uint32_t ApiNumber ; // offset 0x0
    uint16_t ProcessorLevel ; // offset 0x4
    uint16_t Processor ; // offset 0x6
    int32_t ReturnStatus ; // offset 0x8
    union {
        union {
        DBGKD_READ_MEMORY32 ReadMemory ; // offset 0x0
        DBGKD_WRITE_MEMORY32 WriteMemory ; // offset 0x0
        DBGKD_READ_MEMORY64 ReadMemory64 ; // offset 0x0
        DBGKD_WRITE_MEMORY64 WriteMemory64 ; // offset 0x0
        DBGKD_GET_CONTEXT GetContext ; // offset 0x0
        DBGKD_SET_CONTEXT SetContext ; // offset 0x0
        DBGKD_WRITE_BREAKPOINT32 WriteBreakPoint ; // offset 0x0
        DBGKD_RESTORE_BREAKPOINT RestoreBreakPoint ; // offset 0x0
        DBGKD_CONTINUE Continue ; // offset 0x0
        DBGKD_CONTINUE2 Continue2 ; // offset 0x0
        DBGKD_READ_WRITE_IO32 ReadWriteIo ; // offset 0x0
        DBGKD_READ_WRITE_IO_EXTENDED32 ReadWriteIoExtended ; // offset 0x0
        DBGKD_QUERY_SPECIAL_CALLS QuerySpecialCalls ; // offset 0x0
        DBGKD_SET_SPECIAL_CALL32 SetSpecialCall ; // offset 0x0
        DBGKD_SET_INTERNAL_BREAKPOINT32 SetInternalBreakpoint ; // offset 0x0
        DBGKD_GET_INTERNAL_BREAKPOINT32 GetInternalBreakpoint ; // offset 0x0
        DBGKD_GET_VERSION32 GetVersion32 ; // offset 0x0
        DBGKD_BREAKPOINTEX BreakPointEx ; // offset 0x0
        DBGKD_READ_WRITE_MSR ReadWriteMsr ; // offset 0x0
        DBGKD_SEARCH_MEMORY SearchMemory ; // offset 0x0
        };
    } u ; // offset 0xc

} DBGKD_MANIPULATE_STATE32, *PDBGKD_MANIPULATE_STATE32, **PPDBGKD_MANIPULATE_STATE32 ;

typedef struct _IMAGE_SECTION_HEADER { // 0x28 bytes
    uint8_t Name[8] ; // offset 0x0
    union {
        union {
        uint32_t PhysicalAddress ; // offset 0x0
        uint32_t VirtualSize ; // offset 0x0
        };
    } Misc ; // offset 0x8
    uint32_t VirtualAddress ; // offset 0xc
    uint32_t SizeOfRawData ; // offset 0x10
    uint32_t PointerToRawData ; // offset 0x14
    uint32_t PointerToRelocations ; // offset 0x18
    uint32_t PointerToLinenumbers ; // offset 0x1c
    uint16_t NumberOfRelocations ; // offset 0x20
    uint16_t NumberOfLinenumbers ; // offset 0x22
    uint32_t Characteristics ; // offset 0x24

} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER, **PPIMAGE_SECTION_HEADER ;

typedef struct _PCI_INTERFACE { // 0x1c bytes
    uint32_t InterfaceType ; // offset 0x0
    uint16_t MinSize ; // offset 0x4
    uint16_t MinVersion ; // offset 0x6
    uint16_t MaxVersion ; // offset 0x8
    uint16_t Flags ; // offset 0xa
    int32_t ReferenceCount ; // offset 0xc
    PCI_SIGNATURE Signature ; // offset 0x10
    uint32_t Constructor ; // offset 0x14
    uint32_t Initializer ; // offset 0x18

} PCI_INTERFACE, *PPCI_INTERFACE, **PPPCI_INTERFACE ;

typedef struct _RTL_RANGE_LIST { // 0x14 bytes
    LIST_ENTRY ListHead ; // offset 0x0
    uint32_t Flags ; // offset 0x8
    uint32_t Count ; // offset 0xc
    uint32_t Stamp ; // offset 0x10

} RTL_RANGE_LIST, *PRTL_RANGE_LIST, **PPRTL_RANGE_LIST ;

typedef struct _WMI_CLIENT_CONTEXT { // 0x4 bytes
    uint8_t ProcessorNumber ; // offset 0x0
    uint8_t Alignment ; // offset 0x1
    uint16_t LoggerId ; // offset 0x2

} WMI_CLIENT_CONTEXT, *PWMI_CLIENT_CONTEXT, **PPWMI_CLIENT_CONTEXT ;

typedef struct _WNODE_HEADER { // 0x30 bytes
    uint32_t BufferSize ; // offset 0x0
    uint32_t ProviderId ; // offset 0x4
    union {
    uint64_t HistoricalContext ; // offset 0x8
    uint32_t Version ; // offset 0x8
    };
    uint32_t Linkage ; // offset 0xc
    union {
    uint32_t CountLost ; // offset 0x10
    uint32_t KernelHandle ; // offset 0x10
    LARGE_INTEGER TimeStamp ; // offset 0x10
    };
    GUID Guid ; // offset 0x18
    uint32_t ClientContext ; // offset 0x28
    uint32_t Flags ; // offset 0x2c

} WNODE_HEADER, *PWNODE_HEADER, **PPWNODE_HEADER ;

typedef struct _WMI_BUFFER_STATE { // 0x4 bytes
    union {
    uint32_t Free : 1 ; // offset 0x0
    uint32_t InUse : 1 ; // offset 0x0
    uint32_t Flush : 1 ; // offset 0x0
    uint32_t Unused : 29 ; // offset 0x0
    };

} WMI_BUFFER_STATE, *PWMI_BUFFER_STATE, **PPWMI_BUFFER_STATE ;

typedef struct _WMI_BUFFER_HEADER { // 0x48 bytes
    union {
    WNODE_HEADER Wnode ; // offset 0x0
    uint64_t Reserved1 ; // offset 0x0
    };
    uint64_t Reserved2 ; // offset 0x8
    LARGE_INTEGER Reserved3 ; // offset 0x10
    uint32_t Alignment ; // offset 0x18
    SINGLE_LIST_ENTRY SlistEntry ; // offset 0x1c
    LIST_ENTRY Entry ; // offset 0x18
    int32_t ReferenceCount ; // offset 0x0
    uint32_t SavedOffset ; // offset 0x4
    uint32_t CurrentOffset ; // offset 0x8
    uint32_t UsePerfClock ; // offset 0xc
    LARGE_INTEGER TimeStamp ; // offset 0x10
    GUID Guid ; // offset 0x18
    WMI_CLIENT_CONTEXT ClientContext ; // offset 0x28
    union {
    WMI_BUFFER_STATE State ; // offset 0x2c
    uint32_t Flags ; // offset 0x2c
    };
    uint32_t Offset ; // offset 0x30
    uint32_t EventsLost ; // offset 0x34
    union {
    GUID InstanceGuid ; // offset 0x38
    uint32_t LoggerContext ; // offset 0x38
    };
    SINGLE_LIST_ENTRY GlobalEntry ; // offset 0x3c

} WMI_BUFFER_HEADER, *PWMI_BUFFER_HEADER, **PPWMI_BUFFER_HEADER ;

typedef struct _VI_POOL_ENTRY_INUSE { // 0x10 bytes
    uint32_t VirtualAddress ; // offset 0x0
    uint32_t CallingAddress ; // offset 0x4
    uint32_t NumberOfBytes ; // offset 0x8
    uint32_t Tag ; // offset 0xc

} VI_POOL_ENTRY_INUSE, *PVI_POOL_ENTRY_INUSE, **PPVI_POOL_ENTRY_INUSE ;

typedef struct _VI_POOL_ENTRY { // 0x10 bytes
    union {
    VI_POOL_ENTRY_INUSE InUse ; // offset 0x0
    uint32_t FreeListNext ; // offset 0x0
    };

} VI_POOL_ENTRY, *PVI_POOL_ENTRY, **PPVI_POOL_ENTRY ;

typedef struct _EX_RUNDOWN_REF { // 0x4 bytes
    union {
    uint32_t Count ; // offset 0x0
    uint32_t Ptr ; // offset 0x0
    };

} EX_RUNDOWN_REF, *PEX_RUNDOWN_REF, **PPEX_RUNDOWN_REF ;

typedef struct _KEXECUTE_OPTIONS { // 0x1 bytes
    union {
    uint8_t ExecuteDisable : 1 ; // offset 0x0
    uint8_t ExecuteEnable : 1 ; // offset 0x0
    uint8_t DisableThunkEmulation : 1 ; // offset 0x0
    uint8_t Permanent : 1 ; // offset 0x0
    uint8_t ExecuteDispatchEnable : 1 ; // offset 0x0
    uint8_t ImageDispatchEnable : 1 ; // offset 0x0
    uint8_t Spare : 2 ; // offset 0x0
    };

} KEXECUTE_OPTIONS, *PKEXECUTE_OPTIONS, **PPKEXECUTE_OPTIONS ;

typedef struct _KGDTENTRY { // 0x8 bytes
    uint16_t LimitLow ; // offset 0x0
    uint16_t BaseLow ; // offset 0x2
    union {
        union {
        struct {
                    uint8_t BaseMid ; // offset 0x0
                    uint8_t Flags1 ; // offset 0x1
                    uint8_t Flags2 ; // offset 0x2
                    uint8_t BaseHi ; // offset 0x3
                } Bytes ; // offset 0x0
        struct {
                    union {
                    uint32_t BaseMid : 8 ; // offset 0x0
                    uint32_t Type : 5 ; // offset 0x0
                    uint32_t Dpl : 2 ; // offset 0x0
                    uint32_t Pres : 1 ; // offset 0x0
                    uint32_t LimitHi : 4 ; // offset 0x0
                    uint32_t Sys : 1 ; // offset 0x0
                    uint32_t Reserved_0 : 1 ; // offset 0x0
                    uint32_t Default_Big : 1 ; // offset 0x0
                    uint32_t Granularity : 1 ; // offset 0x0
                    uint32_t BaseHi : 8 ; // offset 0x0
                    };
                } Bits ; // offset 0x0
        };
    } HighWord ; // offset 0x4

} KGDTENTRY, *PKGDTENTRY, **PPKGDTENTRY ;

typedef struct _KIDTENTRY { // 0x8 bytes
    uint16_t Offset ; // offset 0x0
    uint16_t Selector ; // offset 0x2
    uint16_t Access ; // offset 0x4
    uint16_t ExtendedOffset ; // offset 0x6

} KIDTENTRY, *PKIDTENTRY, **PPKIDTENTRY ;

typedef struct _KPROCESS { // 0x6c bytes
    DISPATCHER_HEADER Header ; // offset 0x0
    LIST_ENTRY ProfileListHead ; // offset 0x10
    uint32_t DirectoryTableBase[2] ; // offset 0x18
    KGDTENTRY LdtDescriptor ; // offset 0x20
    KIDTENTRY Int21Descriptor ; // offset 0x28
    uint16_t IopmOffset ; // offset 0x30
    uint8_t Iopl ; // offset 0x32
    uint8_t Unused ; // offset 0x33
    uint32_t ActiveProcessors ; // offset 0x34
    uint32_t KernelTime ; // offset 0x38
    uint32_t UserTime ; // offset 0x3c
    LIST_ENTRY ReadyListHead ; // offset 0x40
    SINGLE_LIST_ENTRY SwapListEntry ; // offset 0x48
    uint32_t VdmTrapcHandler ; // offset 0x4c
    LIST_ENTRY ThreadListHead ; // offset 0x50
    uint32_t ProcessLock ; // offset 0x58
    uint32_t Affinity ; // offset 0x5c
    uint16_t StackCount ; // offset 0x60
    uint8_t BasePriority ; // offset 0x62
    uint8_t ThreadQuantum ; // offset 0x63
    uint8_t AutoAlignment ; // offset 0x64
    uint8_t State ; // offset 0x65
    uint8_t ThreadSeed ; // offset 0x66
    uint8_t DisableBoost ; // offset 0x67
    uint8_t PowerState ; // offset 0x68
    uint8_t DisableQuantum ; // offset 0x69
    uint8_t IdealNode ; // offset 0x6a
    union {
    KEXECUTE_OPTIONS Flags ; // offset 0x6b
    uint8_t ExecuteOptions ; // offset 0x6b
    };

} KPROCESS, *PKPROCESS, **PPKPROCESS ;

typedef struct _EX_FAST_REF { // 0x4 bytes
    union {
    uint32_t Object ; // offset 0x0
    uint32_t RefCnt : 3 ; // offset 0x0
    uint32_t Value ; // offset 0x0
    };

} EX_FAST_REF, *PEX_FAST_REF, **PPEX_FAST_REF ;

typedef struct _SE_AUDIT_PROCESS_CREATION_INFO { // 0x4 bytes
    uint32_t ImageFileName ; // offset 0x0

} SE_AUDIT_PROCESS_CREATION_INFO, *PSE_AUDIT_PROCESS_CREATION_INFO, **PPSE_AUDIT_PROCESS_CREATION_INFO ;

typedef struct _EPROCESS { // 0x260 bytes
    KPROCESS Pcb ; // offset 0x0
    EX_PUSH_LOCK ProcessLock ; // offset 0x6c
    LARGE_INTEGER CreateTime ; // offset 0x70
    LARGE_INTEGER ExitTime ; // offset 0x78
    EX_RUNDOWN_REF RundownProtect ; // offset 0x80
    uint32_t UniqueProcessId ; // offset 0x84
    LIST_ENTRY ActiveProcessLinks ; // offset 0x88
    uint32_t QuotaUsage[3] ; // offset 0x90
    uint32_t QuotaPeak[3] ; // offset 0x9c
    uint32_t CommitCharge ; // offset 0xa8
    uint32_t PeakVirtualSize ; // offset 0xac
    uint32_t VirtualSize ; // offset 0xb0
    LIST_ENTRY SessionProcessLinks ; // offset 0xb4
    uint32_t DebugPort ; // offset 0xbc
    uint32_t ExceptionPort ; // offset 0xc0
    uint32_t ObjectTable ; // offset 0xc4
    EX_FAST_REF Token ; // offset 0xc8
    FAST_MUTEX WorkingSetLock ; // offset 0xcc
    uint32_t WorkingSetPage ; // offset 0xec
    FAST_MUTEX AddressCreationLock ; // offset 0xf0
    uint32_t HyperSpaceLock ; // offset 0x110
    uint32_t ForkInProgress ; // offset 0x114
    uint32_t HardwareTrigger ; // offset 0x118
    uint32_t VadRoot ; // offset 0x11c
    uint32_t VadHint ; // offset 0x120
    uint32_t CloneRoot ; // offset 0x124
    uint32_t NumberOfPrivatePages ; // offset 0x128
    uint32_t NumberOfLockedPages ; // offset 0x12c
    uint32_t Win32Process ; // offset 0x130
    uint32_t Job ; // offset 0x134
    uint32_t SectionObject ; // offset 0x138
    uint32_t SectionBaseAddress ; // offset 0x13c
    uint32_t QuotaBlock ; // offset 0x140
    uint32_t WorkingSetWatch ; // offset 0x144
    uint32_t Win32WindowStation ; // offset 0x148
    uint32_t InheritedFromUniqueProcessId ; // offset 0x14c
    uint32_t LdtInformation ; // offset 0x150
    uint32_t VadFreeHint ; // offset 0x154
    uint32_t VdmObjects ; // offset 0x158
    uint32_t DeviceMap ; // offset 0x15c
    LIST_ENTRY PhysicalVadList ; // offset 0x160
    union {
    HARDWARE_PTE PageDirectoryPte ; // offset 0x168
    uint64_t Filler ; // offset 0x168
    };
    uint32_t Session ; // offset 0x170
    uint8_t ImageFileName[16] ; // offset 0x174
    LIST_ENTRY JobLinks ; // offset 0x184
    uint32_t LockedPagesList ; // offset 0x18c
    LIST_ENTRY ThreadListHead ; // offset 0x190
    uint32_t SecurityPort ; // offset 0x198
    uint32_t PaeTop ; // offset 0x19c
    uint32_t ActiveThreads ; // offset 0x1a0
    uint32_t GrantedAccess ; // offset 0x1a4
    uint32_t DefaultHardErrorProcessing ; // offset 0x1a8
    int32_t LastThreadExitStatus ; // offset 0x1ac
    uint32_t Peb ; // offset 0x1b0
    EX_FAST_REF PrefetchTrace ; // offset 0x1b4
    LARGE_INTEGER ReadOperationCount ; // offset 0x1b8
    LARGE_INTEGER WriteOperationCount ; // offset 0x1c0
    LARGE_INTEGER OtherOperationCount ; // offset 0x1c8
    LARGE_INTEGER ReadTransferCount ; // offset 0x1d0
    LARGE_INTEGER WriteTransferCount ; // offset 0x1d8
    LARGE_INTEGER OtherTransferCount ; // offset 0x1e0
    uint32_t CommitChargeLimit ; // offset 0x1e8
    uint32_t CommitChargePeak ; // offset 0x1ec
    uint32_t AweInfo ; // offset 0x1f0
    SE_AUDIT_PROCESS_CREATION_INFO SeAuditProcessCreationInfo ; // offset 0x1f4
    MMSUPPORT Vm ; // offset 0x1f8
    uint32_t LastFaultCount ; // offset 0x238
    uint32_t ModifiedPageCount ; // offset 0x23c
    uint32_t NumberOfVads ; // offset 0x240
    uint32_t JobStatus ; // offset 0x244
    union {
    uint32_t Flags ; // offset 0x248
    uint32_t CreateReported : 1 ; // offset 0x248
    uint32_t NoDebugInherit : 1 ; // offset 0x248
    uint32_t ProcessExiting : 1 ; // offset 0x248
    uint32_t ProcessDelete : 1 ; // offset 0x248
    uint32_t Wow64SplitPages : 1 ; // offset 0x248
    uint32_t VmDeleted : 1 ; // offset 0x248
    uint32_t OutswapEnabled : 1 ; // offset 0x248
    uint32_t Outswapped : 1 ; // offset 0x248
    uint32_t ForkFailed : 1 ; // offset 0x248
    uint32_t HasPhysicalVad : 1 ; // offset 0x248
    uint32_t AddressSpaceInitialized : 2 ; // offset 0x248
    uint32_t SetTimerResolution : 1 ; // offset 0x248
    uint32_t BreakOnTermination : 1 ; // offset 0x248
    uint32_t SessionCreationUnderway : 1 ; // offset 0x248
    uint32_t WriteWatch : 1 ; // offset 0x248
    uint32_t ProcessInSession : 1 ; // offset 0x248
    uint32_t OverrideAddressSpace : 1 ; // offset 0x248
    uint32_t HasAddressSpace : 1 ; // offset 0x248
    uint32_t LaunchPrefetched : 1 ; // offset 0x248
    uint32_t InjectInpageErrors : 1 ; // offset 0x248
    uint32_t VmTopDown : 1 ; // offset 0x248
    uint32_t Unused3 : 1 ; // offset 0x248
    uint32_t Unused4 : 1 ; // offset 0x248
    uint32_t VdmAllowed : 1 ; // offset 0x248
    uint32_t Unused : 5 ; // offset 0x248
    uint32_t Unused1 : 1 ; // offset 0x248
    uint32_t Unused2 : 1 ; // offset 0x248
    };
    int32_t ExitStatus ; // offset 0x24c
    uint16_t NextPageColor ; // offset 0x250
    uint8_t SubSystemMinorVersion ; // offset 0x252
    uint8_t SubSystemMajorVersion ; // offset 0x253
    uint16_t SubSystemVersion ; // offset 0x252
    uint8_t PriorityClass ; // offset 0x254
    uint8_t WorkingSetAcquiredUnsafe ; // offset 0x255
    uint32_t Cookie ; // offset 0x258

} EPROCESS, *PEPROCESS, **PPEPROCESS ;

typedef struct _MI_VERIFIER_DRIVER_ENTRY { // 0x60 bytes
    LIST_ENTRY Links ; // offset 0x0
    uint32_t Loads ; // offset 0x8
    uint32_t Unloads ; // offset 0xc
    UNICODE_STRING BaseName ; // offset 0x10
    uint32_t StartAddress ; // offset 0x18
    uint32_t EndAddress ; // offset 0x1c
    uint32_t Flags ; // offset 0x20
    uint32_t Signature ; // offset 0x24
    uint32_t Reserved ; // offset 0x28
    uint32_t VerifierPoolLock ; // offset 0x2c
    uint32_t PoolHash ; // offset 0x30
    uint32_t PoolHashSize ; // offset 0x34
    uint32_t PoolHashFree ; // offset 0x38
    uint32_t PoolHashReserved ; // offset 0x3c
    uint32_t CurrentPagedPoolAllocations ; // offset 0x40
    uint32_t CurrentNonPagedPoolAllocations ; // offset 0x44
    uint32_t PeakPagedPoolAllocations ; // offset 0x48
    uint32_t PeakNonPagedPoolAllocations ; // offset 0x4c
    uint32_t PagedBytes ; // offset 0x50
    uint32_t NonPagedBytes ; // offset 0x54
    uint32_t PeakPagedBytes ; // offset 0x58
    uint32_t PeakNonPagedBytes ; // offset 0x5c

} MI_VERIFIER_DRIVER_ENTRY, *PMI_VERIFIER_DRIVER_ENTRY, **PPMI_VERIFIER_DRIVER_ENTRY ;

typedef struct _CM_KEY_SECURITY_CACHE { // 0x28 bytes
    uint32_t Cell ; // offset 0x0
    uint32_t ConvKey ; // offset 0x4
    LIST_ENTRY List ; // offset 0x8
    uint32_t DescriptorLength ; // offset 0x10
    SECURITY_DESCRIPTOR_RELATIVE Descriptor ; // offset 0x14

} CM_KEY_SECURITY_CACHE, *PCM_KEY_SECURITY_CACHE, **PPCM_KEY_SECURITY_CACHE ;

typedef struct _EXCEPTION_RECORD32 { // 0x50 bytes
    int32_t ExceptionCode ; // offset 0x0
    uint32_t ExceptionFlags ; // offset 0x4
    uint32_t ExceptionRecord ; // offset 0x8
    uint32_t ExceptionAddress ; // offset 0xc
    uint32_t NumberParameters ; // offset 0x10
    uint32_t ExceptionInformation[15] ; // offset 0x14

} EXCEPTION_RECORD32, *PEXCEPTION_RECORD32, **PPEXCEPTION_RECORD32 ;

typedef struct _DBGKM_EXCEPTION32 { // 0x54 bytes
    EXCEPTION_RECORD32 ExceptionRecord ; // offset 0x0
    uint32_t FirstChance ; // offset 0x50

} DBGKM_EXCEPTION32, *PDBGKM_EXCEPTION32, **PPDBGKM_EXCEPTION32 ;

typedef struct _ARBITER_LIST_ENTRY { // 0x38 bytes
    LIST_ENTRY ListEntry ; // offset 0x0
    uint32_t AlternativeCount ; // offset 0x8
    uint32_t Alternatives ; // offset 0xc
    uint32_t PhysicalDeviceObject ; // offset 0x10
    ARBITER_REQUEST_SOURCE RequestSource ; // offset 0x14
    uint32_t Flags ; // offset 0x18
    int32_t WorkSpace ; // offset 0x1c
    INTERFACE_TYPE InterfaceType ; // offset 0x20
    uint32_t SlotNumber ; // offset 0x24
    uint32_t BusNumber ; // offset 0x28
    uint32_t Assignment ; // offset 0x2c
    uint32_t SelectedAlternative ; // offset 0x30
    ARBITER_RESULT Result ; // offset 0x34

} ARBITER_LIST_ENTRY, *PARBITER_LIST_ENTRY, **PPARBITER_LIST_ENTRY ;

typedef struct _NAMED_PIPE_CREATE_PARAMETERS { // 0x28 bytes
    uint32_t NamedPipeType ; // offset 0x0
    uint32_t ReadMode ; // offset 0x4
    uint32_t CompletionMode ; // offset 0x8
    uint32_t MaximumInstances ; // offset 0xc
    uint32_t InboundQuota ; // offset 0x10
    uint32_t OutboundQuota ; // offset 0x14
    LARGE_INTEGER DefaultTimeout ; // offset 0x18
    uint8_t TimeoutSpecified ; // offset 0x20

} NAMED_PIPE_CREATE_PARAMETERS, *PNAMED_PIPE_CREATE_PARAMETERS, **PPNAMED_PIPE_CREATE_PARAMETERS ;

typedef struct _IO_COUNTERS { // 0x30 bytes
    uint64_t ReadOperationCount ; // offset 0x0
    uint64_t WriteOperationCount ; // offset 0x8
    uint64_t OtherOperationCount ; // offset 0x10
    uint64_t ReadTransferCount ; // offset 0x18
    uint64_t WriteTransferCount ; // offset 0x20
    uint64_t OtherTransferCount ; // offset 0x28

} IO_COUNTERS, *PIO_COUNTERS, **PPIO_COUNTERS ;

typedef struct _EJOB { // 0x180 bytes
    KEVENT Event ; // offset 0x0
    LIST_ENTRY JobLinks ; // offset 0x10
    LIST_ENTRY ProcessListHead ; // offset 0x18
    ERESOURCE JobLock ; // offset 0x20
    LARGE_INTEGER TotalUserTime ; // offset 0x58
    LARGE_INTEGER TotalKernelTime ; // offset 0x60
    LARGE_INTEGER ThisPeriodTotalUserTime ; // offset 0x68
    LARGE_INTEGER ThisPeriodTotalKernelTime ; // offset 0x70
    uint32_t TotalPageFaultCount ; // offset 0x78
    uint32_t TotalProcesses ; // offset 0x7c
    uint32_t ActiveProcesses ; // offset 0x80
    uint32_t TotalTerminatedProcesses ; // offset 0x84
    LARGE_INTEGER PerProcessUserTimeLimit ; // offset 0x88
    LARGE_INTEGER PerJobUserTimeLimit ; // offset 0x90
    uint32_t LimitFlags ; // offset 0x98
    uint32_t MinimumWorkingSetSize ; // offset 0x9c
    uint32_t MaximumWorkingSetSize ; // offset 0xa0
    uint32_t ActiveProcessLimit ; // offset 0xa4
    uint32_t Affinity ; // offset 0xa8
    uint8_t PriorityClass ; // offset 0xac
    uint32_t UIRestrictionsClass ; // offset 0xb0
    uint32_t SecurityLimitFlags ; // offset 0xb4
    uint32_t Token ; // offset 0xb8
    uint32_t Filter ; // offset 0xbc
    uint32_t EndOfJobTimeAction ; // offset 0xc0
    uint32_t CompletionPort ; // offset 0xc4
    uint32_t CompletionKey ; // offset 0xc8
    uint32_t SessionId ; // offset 0xcc
    uint32_t SchedulingClass ; // offset 0xd0
    uint64_t ReadOperationCount ; // offset 0xd8
    uint64_t WriteOperationCount ; // offset 0xe0
    uint64_t OtherOperationCount ; // offset 0xe8
    uint64_t ReadTransferCount ; // offset 0xf0
    uint64_t WriteTransferCount ; // offset 0xf8
    uint64_t OtherTransferCount ; // offset 0x100
    IO_COUNTERS IoInfo ; // offset 0x108
    uint32_t ProcessMemoryLimit ; // offset 0x138
    uint32_t JobMemoryLimit ; // offset 0x13c
    uint32_t PeakProcessMemoryUsed ; // offset 0x140
    uint32_t PeakJobMemoryUsed ; // offset 0x144
    uint32_t CurrentJobMemoryUsed ; // offset 0x148
    FAST_MUTEX MemoryLimitsLock ; // offset 0x14c
    LIST_ENTRY JobSetLinks ; // offset 0x16c
    uint32_t MemberLevel ; // offset 0x174
    uint32_t JobFlags ; // offset 0x178

} EJOB, *PEJOB, **PPEJOB ;

typedef struct _HEAP_UNCOMMMTTED_RANGE { // 0x10 bytes
    uint32_t Next ; // offset 0x0
    uint32_t Address ; // offset 0x4
    uint32_t Size ; // offset 0x8
    uint32_t filler ; // offset 0xc

} HEAP_UNCOMMMTTED_RANGE, *PHEAP_UNCOMMMTTED_RANGE, **PPHEAP_UNCOMMMTTED_RANGE ;

typedef struct _QUAD { // 0x8 bytes
    double DoNotUseThisField ; // offset 0x0

} QUAD, *PQUAD, **PPQUAD ;

typedef struct _OBJECT_HEADER { // 0x20 bytes
    int32_t PointerCount ; // offset 0x0
    union {
    int32_t HandleCount ; // offset 0x4
    uint32_t NextToFree ; // offset 0x4
    };
    uint32_t Type ; // offset 0x8
    uint8_t NameInfoOffset ; // offset 0xc
    uint8_t HandleInfoOffset ; // offset 0xd
    uint8_t QuotaInfoOffset ; // offset 0xe
    uint8_t Flags ; // offset 0xf
    union {
    uint32_t ObjectCreateInfo ; // offset 0x10
    uint32_t QuotaBlockCharged ; // offset 0x10
    };
    uint32_t SecurityDescriptor ; // offset 0x14
    QUAD Body ; // offset 0x18

} OBJECT_HEADER, *POBJECT_HEADER, **PPOBJECT_HEADER ;

typedef struct _POOL_BLOCK_HEAD { // 0x10 bytes
    POOL_HEADER Header ; // offset 0x0
    LIST_ENTRY List ; // offset 0x8

} POOL_BLOCK_HEAD, *PPOOL_BLOCK_HEAD, **PPPOOL_BLOCK_HEAD ;

typedef struct _KAPC_STATE { // 0x18 bytes
    LIST_ENTRY ApcListHead[2] ; // offset 0x0
    uint32_t Process ; // offset 0x10
    uint8_t KernelApcInProgress ; // offset 0x14
    uint8_t KernelApcPending ; // offset 0x15
    uint8_t UserApcPending ; // offset 0x16

} KAPC_STATE, *PKAPC_STATE, **PPKAPC_STATE ;

typedef struct _KWAIT_BLOCK { // 0x18 bytes
    LIST_ENTRY WaitListEntry ; // offset 0x0
    uint32_t Thread ; // offset 0x8
    uint32_t Object ; // offset 0xc
    uint32_t NextWaitBlock ; // offset 0x10
    uint16_t WaitKey ; // offset 0x14
    uint16_t WaitType ; // offset 0x16

} KWAIT_BLOCK, *PKWAIT_BLOCK, **PPKWAIT_BLOCK ;

typedef struct _KTHREAD { // 0x1c0 bytes
    DISPATCHER_HEADER Header ; // offset 0x0
    LIST_ENTRY MutantListHead ; // offset 0x10
    uint32_t InitialStack ; // offset 0x18
    uint32_t StackLimit ; // offset 0x1c
    uint32_t Teb ; // offset 0x20
    uint32_t TlsArray ; // offset 0x24
    uint32_t KernelStack ; // offset 0x28
    uint8_t DebugActive ; // offset 0x2c
    uint8_t State ; // offset 0x2d
    uint8_t Alerted[2] ; // offset 0x2e
    uint8_t Iopl ; // offset 0x30
    uint8_t NpxState ; // offset 0x31
    uint8_t Saturation ; // offset 0x32
    uint8_t Priority ; // offset 0x33
    KAPC_STATE ApcState ; // offset 0x34
    uint32_t ContextSwitches ; // offset 0x4c
    uint8_t IdleSwapBlock ; // offset 0x50
    uint8_t Spare0[3] ; // offset 0x51
    int32_t WaitStatus ; // offset 0x54
    uint8_t WaitIrql ; // offset 0x58
    uint8_t WaitMode ; // offset 0x59
    uint8_t WaitNext ; // offset 0x5a
    uint8_t WaitReason ; // offset 0x5b
    uint32_t WaitBlockList ; // offset 0x5c
    union {
    LIST_ENTRY WaitListEntry ; // offset 0x60
    SINGLE_LIST_ENTRY SwapListEntry ; // offset 0x60
    };
    uint32_t WaitTime ; // offset 0x68
    uint8_t BasePriority ; // offset 0x6c
    uint8_t DecrementCount ; // offset 0x6d
    uint8_t PriorityDecrement ; // offset 0x6e
    uint8_t Quantum ; // offset 0x6f
    KWAIT_BLOCK WaitBlock[4] ; // offset 0x70
    uint32_t LegoData ; // offset 0xd0
    uint32_t KernelApcDisable ; // offset 0xd4
    uint32_t UserAffinity ; // offset 0xd8
    uint8_t SystemAffinityActive ; // offset 0xdc
    uint8_t PowerState ; // offset 0xdd
    uint8_t NpxIrql ; // offset 0xde
    uint8_t InitialNode ; // offset 0xdf
    uint32_t ServiceTable ; // offset 0xe0
    uint32_t Queue ; // offset 0xe4
    uint32_t ApcQueueLock ; // offset 0xe8
    KTIMER Timer ; // offset 0xf0
    LIST_ENTRY QueueListEntry ; // offset 0x118
    uint32_t SoftAffinity ; // offset 0x120
    uint32_t Affinity ; // offset 0x124
    uint8_t Preempted ; // offset 0x128
    uint8_t ProcessReadyQueue ; // offset 0x129
    uint8_t KernelStackResident ; // offset 0x12a
    uint8_t NextProcessor ; // offset 0x12b
    uint32_t CallbackStack ; // offset 0x12c
    uint32_t Win32Thread ; // offset 0x130
    uint32_t TrapFrame ; // offset 0x134
    uint32_t ApcStatePointer[2] ; // offset 0x138
    uint8_t PreviousMode ; // offset 0x140
    uint8_t EnableStackSwap ; // offset 0x141
    uint8_t LargeStack ; // offset 0x142
    uint8_t ResourceIndex ; // offset 0x143
    uint32_t KernelTime ; // offset 0x144
    uint32_t UserTime ; // offset 0x148
    KAPC_STATE SavedApcState ; // offset 0x14c
    uint8_t Alertable ; // offset 0x164
    uint8_t ApcStateIndex ; // offset 0x165
    uint8_t ApcQueueable ; // offset 0x166
    uint8_t AutoAlignment ; // offset 0x167
    uint32_t StackBase ; // offset 0x168
    KAPC SuspendApc ; // offset 0x16c
    KSEMAPHORE SuspendSemaphore ; // offset 0x19c
    LIST_ENTRY ThreadListEntry ; // offset 0x1b0
    uint8_t FreezeCount ; // offset 0x1b8
    uint8_t SuspendCount ; // offset 0x1b9
    uint8_t IdealProcessor ; // offset 0x1ba
    uint8_t DisableBoost ; // offset 0x1bb

} KTHREAD, *PKTHREAD, **PPKTHREAD ;

typedef struct _ETHREAD { // 0x258 bytes
    KTHREAD Tcb ; // offset 0x0
    union {
    LARGE_INTEGER CreateTime ; // offset 0x1c0
    uint32_t NestedFaultCount : 2 ; // offset 0x1c0
    uint32_t ApcNeeded : 1 ; // offset 0x1c0
    };
    union {
    LARGE_INTEGER ExitTime ; // offset 0x1c8
    LIST_ENTRY LpcReplyChain ; // offset 0x1c8
    LIST_ENTRY KeyedWaitChain ; // offset 0x1c8
    };
    union {
    int32_t ExitStatus ; // offset 0x1d0
    uint32_t OfsChain ; // offset 0x1d0
    };
    LIST_ENTRY PostBlockList ; // offset 0x1d4
    union {
    uint32_t TerminationPort ; // offset 0x1dc
    uint32_t ReaperLink ; // offset 0x1dc
    uint32_t KeyedWaitValue ; // offset 0x1dc
    };
    uint32_t ActiveTimerListLock ; // offset 0x1e0
    LIST_ENTRY ActiveTimerListHead ; // offset 0x1e4
    CLIENT_ID Cid ; // offset 0x1ec
    union {
    KSEMAPHORE LpcReplySemaphore ; // offset 0x1f4
    KSEMAPHORE KeyedWaitSemaphore ; // offset 0x1f4
    };
    union {
    uint32_t LpcReplyMessage ; // offset 0x208
    uint32_t LpcWaitingOnPort ; // offset 0x208
    };
    uint32_t ImpersonationInfo ; // offset 0x20c
    LIST_ENTRY IrpList ; // offset 0x210
    uint32_t TopLevelIrp ; // offset 0x218
    uint32_t DeviceToVerify ; // offset 0x21c
    uint32_t ThreadsProcess ; // offset 0x220
    uint32_t StartAddress ; // offset 0x224
    union {
    uint32_t Win32StartAddress ; // offset 0x228
    uint32_t LpcReceivedMessageId ; // offset 0x228
    };
    LIST_ENTRY ThreadListEntry ; // offset 0x22c
    EX_RUNDOWN_REF RundownProtect ; // offset 0x234
    EX_PUSH_LOCK ThreadLock ; // offset 0x238
    uint32_t LpcReplyMessageId ; // offset 0x23c
    uint32_t ReadClusterSize ; // offset 0x240
    uint32_t GrantedAccess ; // offset 0x244
    union {
    uint32_t CrossThreadFlags ; // offset 0x248
    uint32_t Terminated : 1 ; // offset 0x248
    uint32_t DeadThread : 1 ; // offset 0x248
    uint32_t HideFromDebugger : 1 ; // offset 0x248
    uint32_t ActiveImpersonationInfo : 1 ; // offset 0x248
    uint32_t SystemThread : 1 ; // offset 0x248
    uint32_t HardErrorsAreDisabled : 1 ; // offset 0x248
    uint32_t BreakOnTermination : 1 ; // offset 0x248
    uint32_t SkipCreationMsg : 1 ; // offset 0x248
    uint32_t SkipTerminationMsg : 1 ; // offset 0x248
    };
    union {
    uint32_t SameThreadPassiveFlags ; // offset 0x24c
    uint32_t ActiveExWorker : 1 ; // offset 0x24c
    uint32_t ExWorkerCanWaitUser : 1 ; // offset 0x24c
    uint32_t MemoryMaker : 1 ; // offset 0x24c
    };
    union {
    uint32_t SameThreadApcFlags ; // offset 0x250
    uint8_t LpcReceivedMsgIdValid : 1 ; // offset 0x250
    uint8_t LpcExitThreadCalled : 1 ; // offset 0x250
    uint8_t AddressSpaceOwner : 1 ; // offset 0x250
    };
    uint8_t ForwardClusterOnly ; // offset 0x254
    uint8_t DisablePageFaultClustering ; // offset 0x255

} ETHREAD, *PETHREAD, **PPETHREAD ;

typedef struct _PORT_MESSAGE { // 0x18 bytes
    union {
        union {
        struct {
                    int16_t DataLength ; // offset 0x0
                    int16_t TotalLength ; // offset 0x2
                } s1 ; // offset 0x0
        uint32_t Length ; // offset 0x0
        };
    } u1 ; // offset 0x0
    union {
        union {
        struct {
                    int16_t Type ; // offset 0x0
                    int16_t DataInfoOffset ; // offset 0x2
                } s2 ; // offset 0x0
        uint32_t ZeroInit ; // offset 0x0
        };
    } u2 ; // offset 0x4
    union {
    CLIENT_ID ClientId ; // offset 0x8
    double DoNotUseThisField ; // offset 0x8
    };
    uint32_t MessageId ; // offset 0x10
    union {
    uint32_t ClientViewSize ; // offset 0x14
    uint32_t CallbackId ; // offset 0x14
    };

} PORT_MESSAGE, *PPORT_MESSAGE, **PPPORT_MESSAGE ;

typedef struct _LPCP_MESSAGE { // 0x30 bytes
    union {
    LIST_ENTRY Entry ; // offset 0x0
    SINGLE_LIST_ENTRY FreeEntry ; // offset 0x0
    };
    uint32_t Reserved0 ; // offset 0x4
    uint32_t SenderPort ; // offset 0x8
    uint32_t RepliedToThread ; // offset 0xc
    uint32_t PortContext ; // offset 0x10
    PORT_MESSAGE Request ; // offset 0x18

} LPCP_MESSAGE, *PLPCP_MESSAGE, **PPLPCP_MESSAGE ;

typedef struct _MMSECTION_FLAGS { // 0x4 bytes
    union {
    uint32_t BeingDeleted : 1 ; // offset 0x0
    uint32_t BeingCreated : 1 ; // offset 0x0
    uint32_t BeingPurged : 1 ; // offset 0x0
    uint32_t NoModifiedWriting : 1 ; // offset 0x0
    uint32_t FailAllIo : 1 ; // offset 0x0
    uint32_t Image : 1 ; // offset 0x0
    uint32_t Based : 1 ; // offset 0x0
    uint32_t File : 1 ; // offset 0x0
    uint32_t Networked : 1 ; // offset 0x0
    uint32_t NoCache : 1 ; // offset 0x0
    uint32_t PhysicalMemory : 1 ; // offset 0x0
    uint32_t CopyOnWrite : 1 ; // offset 0x0
    uint32_t Reserve : 1 ; // offset 0x0
    uint32_t Commit : 1 ; // offset 0x0
    uint32_t FloppyMedia : 1 ; // offset 0x0
    uint32_t WasPurged : 1 ; // offset 0x0
    uint32_t UserReference : 1 ; // offset 0x0
    uint32_t GlobalMemory : 1 ; // offset 0x0
    uint32_t DeleteOnClose : 1 ; // offset 0x0
    uint32_t FilePointerNull : 1 ; // offset 0x0
    uint32_t DebugSymbolsLoaded : 1 ; // offset 0x0
    uint32_t SetMappedFileIoComplete : 1 ; // offset 0x0
    uint32_t CollidedFlush : 1 ; // offset 0x0
    uint32_t NoChange : 1 ; // offset 0x0
    uint32_t HadUserReference : 1 ; // offset 0x0
    uint32_t ImageMappedInSystemSpace : 1 ; // offset 0x0
    uint32_t UserWritable : 1 ; // offset 0x0
    uint32_t Accessed : 1 ; // offset 0x0
    uint32_t GlobalOnlyPerSession : 1 ; // offset 0x0
    uint32_t Rom : 1 ; // offset 0x0
    uint32_t filler : 2 ; // offset 0x0
    };

} MMSECTION_FLAGS, *PMMSECTION_FLAGS, **PPMMSECTION_FLAGS ;

typedef struct _LARGE_CONTROL_AREA { // 0x40 bytes
    uint32_t Segment ; // offset 0x0
    LIST_ENTRY DereferenceList ; // offset 0x4
    uint32_t NumberOfSectionReferences ; // offset 0xc
    uint32_t NumberOfPfnReferences ; // offset 0x10
    uint32_t NumberOfMappedViews ; // offset 0x14
    uint16_t NumberOfSubsections ; // offset 0x18
    uint16_t FlushInProgressCount ; // offset 0x1a
    uint32_t NumberOfUserReferences ; // offset 0x1c
    union {
        union {
        uint32_t LongFlags ; // offset 0x0
        MMSECTION_FLAGS Flags ; // offset 0x0
        };
    } u ; // offset 0x20
    uint32_t FilePointer ; // offset 0x24
    uint32_t WaitingForDeletion ; // offset 0x28
    uint16_t ModifiedWriteCount ; // offset 0x2c
    uint16_t NumberOfSystemCacheViews ; // offset 0x2e
    uint32_t StartingFrame ; // offset 0x30
    LIST_ENTRY UserGlobalList ; // offset 0x34
    uint32_t SessionId ; // offset 0x3c

} LARGE_CONTROL_AREA, *PLARGE_CONTROL_AREA, **PPLARGE_CONTROL_AREA ;

typedef struct _DEVICE_NODE { // 0x118 bytes
    uint32_t Sibling ; // offset 0x0
    uint32_t Child ; // offset 0x4
    uint32_t Parent ; // offset 0x8
    uint32_t LastChild ; // offset 0xc
    uint32_t Level ; // offset 0x10
    uint32_t Notify ; // offset 0x14
    PNP_DEVNODE_STATE State ; // offset 0x18
    PNP_DEVNODE_STATE PreviousState ; // offset 0x1c
    PNP_DEVNODE_STATE StateHistory[20] ; // offset 0x20
    uint32_t StateHistoryEntry ; // offset 0x70
    int32_t CompletionStatus ; // offset 0x74
    uint32_t PendingIrp ; // offset 0x78
    uint32_t Flags ; // offset 0x7c
    uint32_t UserFlags ; // offset 0x80
    uint32_t Problem ; // offset 0x84
    uint32_t PhysicalDeviceObject ; // offset 0x88
    uint32_t ResourceList ; // offset 0x8c
    uint32_t ResourceListTranslated ; // offset 0x90
    UNICODE_STRING InstancePath ; // offset 0x94
    UNICODE_STRING ServiceName ; // offset 0x9c
    uint32_t DuplicatePDO ; // offset 0xa4
    uint32_t ResourceRequirements ; // offset 0xa8
    INTERFACE_TYPE InterfaceType ; // offset 0xac
    uint32_t BusNumber ; // offset 0xb0
    INTERFACE_TYPE ChildInterfaceType ; // offset 0xb4
    uint32_t ChildBusNumber ; // offset 0xb8
    uint16_t ChildBusTypeIndex ; // offset 0xbc
    uint8_t RemovalPolicy ; // offset 0xbe
    uint8_t HardwareRemovalPolicy ; // offset 0xbf
    LIST_ENTRY TargetDeviceNotify ; // offset 0xc0
    LIST_ENTRY DeviceArbiterList ; // offset 0xc8
    LIST_ENTRY DeviceTranslatorList ; // offset 0xd0
    uint16_t NoTranslatorMask ; // offset 0xd8
    uint16_t QueryTranslatorMask ; // offset 0xda
    uint16_t NoArbiterMask ; // offset 0xdc
    uint16_t QueryArbiterMask ; // offset 0xde
    union {
        union {
        uint32_t LegacyDeviceNode ; // offset 0x0
        uint32_t PendingDeviceRelations ; // offset 0x0
        };
    } OverUsed1 ; // offset 0xe0
    union {
        uint32_t NextResourceDeviceNode ; // offset 0x0
    } OverUsed2 ; // offset 0xe4
    uint32_t BootResources ; // offset 0xe8
    uint32_t CapabilityFlags ; // offset 0xec
    struct {
        PROFILE_STATUS DockStatus ; // offset 0x0
        LIST_ENTRY ListEntry ; // offset 0x4
        uint32_t SerialNumber ; // offset 0xc
    } DockInfo ; // offset 0xf0
    uint32_t DisableableDepends ; // offset 0x100
    LIST_ENTRY PendedSetInterfaceState ; // offset 0x104
    LIST_ENTRY LegacyBusListEntry ; // offset 0x10c
    uint32_t DriverUnloadRetryCount ; // offset 0x114

} DEVICE_NODE, *PDEVICE_NODE, **PPDEVICE_NODE ;

typedef struct _PI_RESOURCE_ARBITER_ENTRY { // 0x38 bytes
    LIST_ENTRY DeviceArbiterList ; // offset 0x0
    uint8_t ResourceType ; // offset 0x8
    uint32_t ArbiterInterface ; // offset 0xc
    uint32_t Level ; // offset 0x10
    LIST_ENTRY ResourceList ; // offset 0x14
    LIST_ENTRY BestResourceList ; // offset 0x1c
    LIST_ENTRY BestConfig ; // offset 0x24
    LIST_ENTRY ActiveArbiterList ; // offset 0x2c
    uint8_t State ; // offset 0x34
    uint8_t ResourcesChanged ; // offset 0x35

} PI_RESOURCE_ARBITER_ENTRY, *PPI_RESOURCE_ARBITER_ENTRY, **PPPI_RESOURCE_ARBITER_ENTRY ;

typedef struct _EVENT_COUNTER { // 0x18 bytes
    SINGLE_LIST_ENTRY ListEntry ; // offset 0x0
    uint32_t RefCount ; // offset 0x4
    KEVENT Event ; // offset 0x8

} EVENT_COUNTER, *PEVENT_COUNTER, **PPEVENT_COUNTER ;

typedef struct _CONTROL_AREA { // 0x30 bytes
    uint32_t Segment ; // offset 0x0
    LIST_ENTRY DereferenceList ; // offset 0x4
    uint32_t NumberOfSectionReferences ; // offset 0xc
    uint32_t NumberOfPfnReferences ; // offset 0x10
    uint32_t NumberOfMappedViews ; // offset 0x14
    uint16_t NumberOfSubsections ; // offset 0x18
    uint16_t FlushInProgressCount ; // offset 0x1a
    uint32_t NumberOfUserReferences ; // offset 0x1c
    union {
        union {
        uint32_t LongFlags ; // offset 0x0
        MMSECTION_FLAGS Flags ; // offset 0x0
        };
    } u ; // offset 0x20
    uint32_t FilePointer ; // offset 0x24
    uint32_t WaitingForDeletion ; // offset 0x28
    uint16_t ModifiedWriteCount ; // offset 0x2c
    uint16_t NumberOfSystemCacheViews ; // offset 0x2e

} CONTROL_AREA, *PCONTROL_AREA, **PPCONTROL_AREA ;

typedef struct _PEB_LDR_DATA { // 0x28 bytes
    uint32_t Length ; // offset 0x0
    uint8_t Initialized ; // offset 0x4
    uint32_t SsHandle ; // offset 0x8
    LIST_ENTRY InLoadOrderModuleList ; // offset 0xc
    LIST_ENTRY InMemoryOrderModuleList ; // offset 0x14
    LIST_ENTRY InInitializationOrderModuleList ; // offset 0x1c
    uint32_t EntryInProgress ; // offset 0x24

} PEB_LDR_DATA, *PPEB_LDR_DATA, **PPPEB_LDR_DATA ;

typedef struct _ARBITER_ALTERNATIVE { // 0x30 bytes
    uint64_t Minimum ; // offset 0x0
    uint64_t Maximum ; // offset 0x8
    uint32_t Length ; // offset 0x10
    uint32_t Alignment ; // offset 0x14
    int32_t Priority ; // offset 0x18
    uint32_t Flags ; // offset 0x1c
    uint32_t Descriptor ; // offset 0x20
    uint32_t Reserved[3] ; // offset 0x24

} ARBITER_ALTERNATIVE, *PARBITER_ALTERNATIVE, **PPARBITER_ALTERNATIVE ;

typedef struct _ARBITER_ALLOCATION_STATE { // 0x38 bytes
    uint64_t Start ; // offset 0x0
    uint64_t End ; // offset 0x8
    uint64_t CurrentMinimum ; // offset 0x10
    uint64_t CurrentMaximum ; // offset 0x18
    uint32_t Entry ; // offset 0x20
    uint32_t CurrentAlternative ; // offset 0x24
    uint32_t AlternativeCount ; // offset 0x28
    uint32_t Alternatives ; // offset 0x2c
    uint16_t Flags ; // offset 0x30
    uint8_t RangeAttributes ; // offset 0x32
    uint8_t RangeAvailableAttributes ; // offset 0x33
    uint32_t WorkSpace ; // offset 0x34

} ARBITER_ALLOCATION_STATE, *PARBITER_ALLOCATION_STATE, **PPARBITER_ALLOCATION_STATE ;

typedef struct _PEB_FREE_BLOCK { // 0x8 bytes
    uint32_t Next ; // offset 0x0
    uint32_t Size ; // offset 0x4

} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK, **PPPEB_FREE_BLOCK ;

typedef struct _PROCESSOR_PERF_STATE { // 0x20 bytes
    uint8_t PercentFrequency ; // offset 0x0
    uint8_t MinCapacity ; // offset 0x1
    uint16_t Power ; // offset 0x2
    uint8_t IncreaseLevel ; // offset 0x4
    uint8_t DecreaseLevel ; // offset 0x5
    uint16_t Flags ; // offset 0x6
    uint32_t IncreaseTime ; // offset 0x8
    uint32_t DecreaseTime ; // offset 0xc
    uint32_t IncreaseCount ; // offset 0x10
    uint32_t DecreaseCount ; // offset 0x14
    uint64_t PerformanceTime ; // offset 0x18

} PROCESSOR_PERF_STATE, *PPROCESSOR_PERF_STATE, **PPPROCESSOR_PERF_STATE ;

typedef struct _RTL_RANGE { // 0x20 bytes
    uint64_t Start ; // offset 0x0
    uint64_t End ; // offset 0x8
    uint32_t UserData ; // offset 0x10
    uint32_t Owner ; // offset 0x14
    uint8_t Attributes ; // offset 0x18
    uint8_t Flags ; // offset 0x19

} RTL_RANGE, *PRTL_RANGE, **PPRTL_RANGE ;

typedef struct _TRACE_ENABLE_FLAG_EXTENSION { // 0x4 bytes
    uint16_t Offset ; // offset 0x0
    uint8_t Length ; // offset 0x2
    uint8_t Flag ; // offset 0x3

} TRACE_ENABLE_FLAG_EXTENSION, *PTRACE_ENABLE_FLAG_EXTENSION, **PPTRACE_ENABLE_FLAG_EXTENSION ;

typedef struct _WMI_LOGGER_MODE { // 0x4 bytes
    union {
    uint32_t SequentialFile : 1 ; // offset 0x0
    uint32_t CircularFile : 1 ; // offset 0x0
    uint32_t AppendFile : 1 ; // offset 0x0
    uint32_t Unused1 : 5 ; // offset 0x0
    uint32_t RealTime : 1 ; // offset 0x0
    uint32_t DelayOpenFile : 1 ; // offset 0x0
    uint32_t BufferOnly : 1 ; // offset 0x0
    uint32_t PrivateLogger : 1 ; // offset 0x0
    uint32_t AddHeader : 1 ; // offset 0x0
    uint32_t UseExisting : 1 ; // offset 0x0
    uint32_t UseGlobalSequence : 1 ; // offset 0x0
    uint32_t UseLocalSequence : 1 ; // offset 0x0
    uint32_t Unused2 : 16 ; // offset 0x0
    };

} WMI_LOGGER_MODE, *PWMI_LOGGER_MODE, **PPWMI_LOGGER_MODE ;

typedef struct _WMI_LOGGER_CONTEXT { // 0x1c8 bytes
    uint32_t BufferSpinLock ; // offset 0x0
    LARGE_INTEGER StartTime ; // offset 0x8
    uint32_t LogFileHandle ; // offset 0x10
    KSEMAPHORE LoggerSemaphore ; // offset 0x14
    uint32_t LoggerThread ; // offset 0x28
    KEVENT LoggerEvent ; // offset 0x2c
    KEVENT FlushEvent ; // offset 0x3c
    int32_t LoggerStatus ; // offset 0x4c
    uint32_t LoggerId ; // offset 0x50
    int32_t BuffersAvailable ; // offset 0x54
    uint32_t UsePerfClock ; // offset 0x58
    uint32_t WriteFailureLimit ; // offset 0x5c
    uint32_t BuffersDirty ; // offset 0x60
    uint32_t BuffersInUse ; // offset 0x64
    uint32_t SwitchingInProgress ; // offset 0x68
    SLIST_HEADER FreeList ; // offset 0x70
    SLIST_HEADER FlushList ; // offset 0x78
    SLIST_HEADER GlobalList ; // offset 0x80
    uint32_t ProcessorBuffers ; // offset 0x88
    UNICODE_STRING LoggerName ; // offset 0x8c
    UNICODE_STRING LogFileName ; // offset 0x94
    UNICODE_STRING LogFilePattern ; // offset 0x9c
    UNICODE_STRING NewLogFileName ; // offset 0xa4
    uint32_t EndPageMarker ; // offset 0xac
    int32_t CollectionOn ; // offset 0xb0
    uint32_t KernelTraceOn ; // offset 0xb4
    int32_t PerfLogInTransition ; // offset 0xb8
    uint32_t RequestFlag ; // offset 0xbc
    uint32_t EnableFlags ; // offset 0xc0
    uint32_t MaximumFileSize ; // offset 0xc4
    union {
    uint32_t LoggerMode ; // offset 0xc8
    WMI_LOGGER_MODE LoggerModeFlags ; // offset 0xc8
    };
    uint32_t LastFlushedBuffer ; // offset 0xcc
    uint32_t RefCount ; // offset 0xd0
    uint32_t FlushTimer ; // offset 0xd4
    LARGE_INTEGER FirstBufferOffset ; // offset 0xd8
    LARGE_INTEGER ByteOffset ; // offset 0xe0
    LARGE_INTEGER BufferAgeLimit ; // offset 0xe8
    uint32_t MaximumBuffers ; // offset 0xf0
    uint32_t MinimumBuffers ; // offset 0xf4
    uint32_t EventsLost ; // offset 0xf8
    uint32_t BuffersWritten ; // offset 0xfc
    uint32_t LogBuffersLost ; // offset 0x100
    uint32_t RealTimeBuffersLost ; // offset 0x104
    uint32_t BufferSize ; // offset 0x108
    int32_t NumberOfBuffers ; // offset 0x10c
    uint32_t SequencePtr ; // offset 0x110
    GUID InstanceGuid ; // offset 0x114
    uint32_t LoggerHeader ; // offset 0x124
    uint32_t GetCpuClock ; // offset 0x128
    SECURITY_CLIENT_CONTEXT ClientSecurityContext ; // offset 0x12c
    uint32_t LoggerExtension ; // offset 0x168
    int32_t ReleaseQueue ; // offset 0x16c
    TRACE_ENABLE_FLAG_EXTENSION EnableFlagExtension ; // offset 0x170
    uint32_t LocalSequence ; // offset 0x174
    uint32_t MaximumIrql ; // offset 0x178
    uint32_t EnableFlagArray ; // offset 0x17c
    KMUTANT LoggerMutex ; // offset 0x180
    int32_t MutexCount ; // offset 0x1a0
    uint32_t FileCounter ; // offset 0x1a4
    uint32_t BufferCallback ; // offset 0x1a8
    uint32_t CallbackContext ; // offset 0x1ac
    POOL_TYPE PoolType ; // offset 0x1b0
    LARGE_INTEGER ReferenceSystemTime ; // offset 0x1b8
    LARGE_INTEGER ReferenceTimeStamp ; // offset 0x1c0

} WMI_LOGGER_CONTEXT, *PWMI_LOGGER_CONTEXT, **PPWMI_LOGGER_CONTEXT ;

typedef struct _IO_CLIENT_EXTENSION { // 0x8 bytes
    uint32_t NextExtension ; // offset 0x0
    uint32_t ClientIdentificationAddress ; // offset 0x4

} IO_CLIENT_EXTENSION, *PIO_CLIENT_EXTENSION, **PPIO_CLIENT_EXTENSION ;

typedef struct _BATTERY_REPORTING_SCALE { // 0x8 bytes
    uint32_t Granularity ; // offset 0x0
    uint32_t Capacity ; // offset 0x4

} BATTERY_REPORTING_SCALE, *PBATTERY_REPORTING_SCALE, **PPBATTERY_REPORTING_SCALE ;

typedef struct _SYSTEM_POWER_CAPABILITIES { // 0x4c bytes
    uint8_t PowerButtonPresent ; // offset 0x0
    uint8_t SleepButtonPresent ; // offset 0x1
    uint8_t LidPresent ; // offset 0x2
    uint8_t SystemS1 ; // offset 0x3
    uint8_t SystemS2 ; // offset 0x4
    uint8_t SystemS3 ; // offset 0x5
    uint8_t SystemS4 ; // offset 0x6
    uint8_t SystemS5 ; // offset 0x7
    uint8_t HiberFilePresent ; // offset 0x8
    uint8_t FullWake ; // offset 0x9
    uint8_t VideoDimPresent ; // offset 0xa
    uint8_t ApmPresent ; // offset 0xb
    uint8_t UpsPresent ; // offset 0xc
    uint8_t ThermalControl ; // offset 0xd
    uint8_t ProcessorThrottle ; // offset 0xe
    uint8_t ProcessorMinThrottle ; // offset 0xf
    uint8_t ProcessorMaxThrottle ; // offset 0x10
    uint8_t spare2[4] ; // offset 0x11
    uint8_t DiskSpinDown ; // offset 0x15
    uint8_t spare3[8] ; // offset 0x16
    uint8_t SystemBatteriesPresent ; // offset 0x1e
    uint8_t BatteriesAreShortTerm ; // offset 0x1f
    BATTERY_REPORTING_SCALE BatteryScale[3] ; // offset 0x20
    SYSTEM_POWER_STATE AcOnLineWake ; // offset 0x38
    SYSTEM_POWER_STATE SoftLidWake ; // offset 0x3c
    SYSTEM_POWER_STATE RtcWake ; // offset 0x40
    SYSTEM_POWER_STATE MinDeviceWakeState ; // offset 0x44
    SYSTEM_POWER_STATE DefaultLowLatencyWake ; // offset 0x48

} SYSTEM_POWER_CAPABILITIES, *PSYSTEM_POWER_CAPABILITIES, **PPSYSTEM_POWER_CAPABILITIES ;

typedef struct _HANDLE_TRACE_DB_ENTRY { // 0x50 bytes
    CLIENT_ID ClientId ; // offset 0x0
    uint32_t Handle ; // offset 0x8
    uint32_t Type ; // offset 0xc
    uint32_t StackTrace[16] ; // offset 0x10

} HANDLE_TRACE_DB_ENTRY, *PHANDLE_TRACE_DB_ENTRY, **PPHANDLE_TRACE_DB_ENTRY ;

typedef struct _HANDLE_TRACE_DEBUG_INFO { // 0x50004 bytes
    uint32_t CurrentStackIndex ; // offset 0x0
    HANDLE_TRACE_DB_ENTRY TraceDb[4096] ; // offset 0x4

} HANDLE_TRACE_DEBUG_INFO, *PHANDLE_TRACE_DEBUG_INFO, **PPHANDLE_TRACE_DEBUG_INFO ;

typedef struct _FILE_STANDARD_INFORMATION { // 0x18 bytes
    LARGE_INTEGER AllocationSize ; // offset 0x0
    LARGE_INTEGER EndOfFile ; // offset 0x8
    uint32_t NumberOfLinks ; // offset 0x10
    uint8_t DeletePending ; // offset 0x14
    uint8_t Directory ; // offset 0x15

} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION, **PPFILE_STANDARD_INFORMATION ;

typedef struct _LIST_ENTRY32 { // 0x8 bytes
    uint32_t Flink ; // offset 0x0
    uint32_t Blink ; // offset 0x4

} LIST_ENTRY32, *PLIST_ENTRY32, **PPLIST_ENTRY32 ;

typedef struct _IO_COMPLETION_CONTEXT { // 0x8 bytes
    uint32_t Port ; // offset 0x0
    uint32_t Key ; // offset 0x4

} IO_COMPLETION_CONTEXT, *PIO_COMPLETION_CONTEXT, **PPIO_COMPLETION_CONTEXT ;

typedef struct _DRIVER_EXTENSION { // 0x1c bytes
    uint32_t DriverObject ; // offset 0x0
    uint32_t AddDevice ; // offset 0x4
    uint32_t Count ; // offset 0x8
    UNICODE_STRING ServiceKeyName ; // offset 0xc
    uint32_t ClientDriverExtension ; // offset 0x14
    uint32_t FsFilterCallbacks ; // offset 0x18

} DRIVER_EXTENSION, *PDRIVER_EXTENSION, **PPDRIVER_EXTENSION ;

typedef struct _VPB { // 0x58 bytes
    int16_t Type ; // offset 0x0
    int16_t Size ; // offset 0x2
    uint16_t Flags ; // offset 0x4
    uint16_t VolumeLabelLength ; // offset 0x6
    uint32_t DeviceObject ; // offset 0x8
    uint32_t RealDevice ; // offset 0xc
    uint32_t SerialNumber ; // offset 0x10
    uint32_t ReferenceCount ; // offset 0x14
    uint16_t VolumeLabel[32] ; // offset 0x18

} VPB, *PVPB, **PPVPB ;

typedef struct _CM_NOTIFY_BLOCK { // 0x2c bytes
    LIST_ENTRY HiveList ; // offset 0x0
    LIST_ENTRY PostList ; // offset 0x8
    uint32_t KeyControlBlock ; // offset 0x10
    uint32_t KeyBody ; // offset 0x14
    union {
    uint32_t Filter : 30 ; // offset 0x18
    uint32_t WatchTree : 1 ; // offset 0x18
    uint32_t NotifyPending : 1 ; // offset 0x18
    };
    SECURITY_SUBJECT_CONTEXT SubjectContext ; // offset 0x1c

} CM_NOTIFY_BLOCK, *PCM_NOTIFY_BLOCK, **PPCM_NOTIFY_BLOCK ;

typedef struct _HEAP_TAG_ENTRY { // 0x40 bytes
    uint32_t Allocs ; // offset 0x0
    uint32_t Frees ; // offset 0x4
    uint32_t Size ; // offset 0x8
    uint16_t TagIndex ; // offset 0xc
    uint16_t CreatorBackTraceIndex ; // offset 0xe
    uint16_t TagName[24] ; // offset 0x10

} HEAP_TAG_ENTRY, *PHEAP_TAG_ENTRY, **PPHEAP_TAG_ENTRY ;

typedef struct _IMAGE_ROM_OPTIONAL_HEADER { // 0x38 bytes
    uint16_t Magic ; // offset 0x0
    uint8_t MajorLinkerVersion ; // offset 0x2
    uint8_t MinorLinkerVersion ; // offset 0x3
    uint32_t SizeOfCode ; // offset 0x4
    uint32_t SizeOfInitializedData ; // offset 0x8
    uint32_t SizeOfUninitializedData ; // offset 0xc
    uint32_t AddressOfEntryPoint ; // offset 0x10
    uint32_t BaseOfCode ; // offset 0x14
    uint32_t BaseOfData ; // offset 0x18
    uint32_t BaseOfBss ; // offset 0x1c
    uint32_t GprMask ; // offset 0x20
    uint32_t CprMask[4] ; // offset 0x24
    uint32_t GpValue ; // offset 0x34

} IMAGE_ROM_OPTIONAL_HEADER, *PIMAGE_ROM_OPTIONAL_HEADER, **PPIMAGE_ROM_OPTIONAL_HEADER ;

typedef struct _SID_AND_ATTRIBUTES { // 0x8 bytes
    uint32_t Sid ; // offset 0x0
    uint32_t Attributes ; // offset 0x4

} SID_AND_ATTRIBUTES, *PSID_AND_ATTRIBUTES, **PPSID_AND_ATTRIBUTES ;

typedef struct _MMVAD { // 0x28 bytes
    uint32_t StartingVpn ; // offset 0x0
    uint32_t EndingVpn ; // offset 0x4
    uint32_t Parent ; // offset 0x8
    uint32_t LeftChild ; // offset 0xc
    uint32_t RightChild ; // offset 0x10
    union {
        union {
        uint32_t LongFlags ; // offset 0x0
        MMVAD_FLAGS VadFlags ; // offset 0x0
        };
    } u ; // offset 0x14
    uint32_t ControlArea ; // offset 0x18
    uint32_t FirstPrototypePte ; // offset 0x1c
    uint32_t LastContiguousPte ; // offset 0x20
    union {
        union {
        uint32_t LongFlags2 ; // offset 0x0
        MMVAD_FLAGS2 VadFlags2 ; // offset 0x0
        };
    } u2 ; // offset 0x24

} MMVAD, *PMMVAD, **PPMMVAD ;

typedef struct _HBASE_BLOCK { // 0x1000 bytes
    uint32_t Signature ; // offset 0x0
    uint32_t Sequence1 ; // offset 0x4
    uint32_t Sequence2 ; // offset 0x8
    LARGE_INTEGER TimeStamp ; // offset 0xc
    uint32_t Major ; // offset 0x14
    uint32_t Minor ; // offset 0x18
    uint32_t Type ; // offset 0x1c
    uint32_t Format ; // offset 0x20
    uint32_t RootCell ; // offset 0x24
    uint32_t Length ; // offset 0x28
    uint32_t Cluster ; // offset 0x2c
    uint8_t FileName[64] ; // offset 0x30
    uint32_t Reserved1[99] ; // offset 0x70
    uint32_t CheckSum ; // offset 0x1fc
    uint32_t Reserved2[894] ; // offset 0x200
    uint32_t BootType ; // offset 0xff8
    uint32_t BootRecover ; // offset 0xffc

} HBASE_BLOCK, *PHBASE_BLOCK, **PPHBASE_BLOCK ;

typedef struct _INTERLOCK_SEQ { // 0x8 bytes
    uint16_t Depth ; // offset 0x0
    uint16_t FreeEntryOffset ; // offset 0x2
    volatile uint32_t OffsetAndDepth ; // offset 0x0
    volatile uint32_t Sequence ; // offset 0x4
    volatile int64_t Exchg ; // offset 0x0

} INTERLOCK_SEQ, *PINTERLOCK_SEQ, **PPINTERLOCK_SEQ ;

typedef struct _HEAP_SUBSEGMENT { // 0x20 bytes
    uint32_t Bucket ; // offset 0x0
    uint32_t UserBlocks ; // offset 0x4
    INTERLOCK_SEQ AggregateExchg ; // offset 0x8
    uint16_t BlockSize ; // offset 0x10
    uint16_t FreeThreshold ; // offset 0x12
    uint16_t BlockCount ; // offset 0x14
    uint8_t SizeIndex ; // offset 0x16
    uint8_t AffinityIndex ; // offset 0x17
    uint32_t Alignment[2] ; // offset 0x10
    SINGLE_LIST_ENTRY SFreeListEntry ; // offset 0x18
    volatile uint32_t Lock ; // offset 0x1c

} HEAP_SUBSEGMENT, *PHEAP_SUBSEGMENT, **PPHEAP_SUBSEGMENT ;

typedef struct _OBJECT_CREATE_INFORMATION { // 0x30 bytes
    uint32_t Attributes ; // offset 0x0
    uint32_t RootDirectory ; // offset 0x4
    uint32_t ParseContext ; // offset 0x8
    uint8_t ProbeMode ; // offset 0xc
    uint32_t PagedPoolCharge ; // offset 0x10
    uint32_t NonPagedPoolCharge ; // offset 0x14
    uint32_t SecurityDescriptorCharge ; // offset 0x18
    uint32_t SecurityDescriptor ; // offset 0x1c
    uint32_t SecurityQos ; // offset 0x20
    SECURITY_QUALITY_OF_SERVICE SecurityQualityOfService ; // offset 0x24

} OBJECT_CREATE_INFORMATION, *POBJECT_CREATE_INFORMATION, **PPOBJECT_CREATE_INFORMATION ;

typedef struct _IMAGE_DEBUG_DIRECTORY { // 0x1c bytes
    uint32_t Characteristics ; // offset 0x0
    uint32_t TimeDateStamp ; // offset 0x4
    uint16_t MajorVersion ; // offset 0x8
    uint16_t MinorVersion ; // offset 0xa
    uint32_t Type ; // offset 0xc
    uint32_t SizeOfData ; // offset 0x10
    uint32_t AddressOfRawData ; // offset 0x14
    uint32_t PointerToRawData ; // offset 0x18

} IMAGE_DEBUG_DIRECTORY, *PIMAGE_DEBUG_DIRECTORY, **PPIMAGE_DEBUG_DIRECTORY ;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT { // 0x8 bytes
    uint32_t Flags ; // offset 0x0
    uint32_t FrameName ; // offset 0x4

} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT, **PPTEB_ACTIVE_FRAME_CONTEXT ;

typedef struct _KINTERRUPT { // 0x1e4 bytes
    int16_t Type ; // offset 0x0
    int16_t Size ; // offset 0x2
    LIST_ENTRY InterruptListEntry ; // offset 0x4
    uint32_t ServiceRoutine ; // offset 0xc
    uint32_t ServiceContext ; // offset 0x10
    uint32_t SpinLock ; // offset 0x14
    uint32_t TickCount ; // offset 0x18
    uint32_t ActualLock ; // offset 0x1c
    uint32_t DispatchAddress ; // offset 0x20
    uint32_t Vector ; // offset 0x24
    uint8_t Irql ; // offset 0x28
    uint8_t SynchronizeIrql ; // offset 0x29
    uint8_t FloatingSave ; // offset 0x2a
    uint8_t Connected ; // offset 0x2b
    uint8_t Number ; // offset 0x2c
    uint8_t ShareVector ; // offset 0x2d
    KINTERRUPT_MODE Mode ; // offset 0x30
    uint32_t ServiceCount ; // offset 0x34
    uint32_t DispatchCount ; // offset 0x38
    uint32_t DispatchCode[106] ; // offset 0x3c

} KINTERRUPT, *PKINTERRUPT, **PPKINTERRUPT ;

typedef struct _DUAL { // 0xdc bytes
    uint32_t Length ; // offset 0x0
    uint32_t Map ; // offset 0x4
    uint32_t SmallDir ; // offset 0x8
    uint32_t Guard ; // offset 0xc
    RTL_BITMAP FreeDisplay[24] ; // offset 0x10
    uint32_t FreeSummary ; // offset 0xd0
    LIST_ENTRY FreeBins ; // offset 0xd4

} DUAL, *PDUAL, **PPDUAL ;

typedef struct _HHIVE { // 0x210 bytes
    uint32_t Signature ; // offset 0x0
    uint32_t GetCellRoutine ; // offset 0x4
    uint32_t ReleaseCellRoutine ; // offset 0x8
    uint32_t Allocate ; // offset 0xc
    uint32_t Free ; // offset 0x10
    uint32_t FileSetSize ; // offset 0x14
    uint32_t FileWrite ; // offset 0x18
    uint32_t FileRead ; // offset 0x1c
    uint32_t FileFlush ; // offset 0x20
    uint32_t BaseBlock ; // offset 0x24
    RTL_BITMAP DirtyVector ; // offset 0x28
    uint32_t DirtyCount ; // offset 0x30
    uint32_t DirtyAlloc ; // offset 0x34
    uint8_t RealWrites ; // offset 0x38
    uint32_t Cluster ; // offset 0x3c
    uint8_t Flat ; // offset 0x40
    uint8_t ReadOnly ; // offset 0x41
    uint8_t Log ; // offset 0x42
    uint32_t HiveFlags ; // offset 0x44
    uint32_t LogSize ; // offset 0x48
    uint32_t RefreshCount ; // offset 0x4c
    uint32_t StorageTypeCount ; // offset 0x50
    uint32_t Version ; // offset 0x54
    DUAL Storage[2] ; // offset 0x58

} HHIVE, *PHHIVE, **PPHHIVE ;

typedef struct _CMHIVE { // 0x49c bytes
    HHIVE Hive ; // offset 0x0
    uint32_t FileHandles[3] ; // offset 0x210
    LIST_ENTRY NotifyList ; // offset 0x21c
    LIST_ENTRY HiveList ; // offset 0x224
    uint32_t HiveLock ; // offset 0x22c
    uint32_t ViewLock ; // offset 0x230
    LIST_ENTRY LRUViewListHead ; // offset 0x234
    LIST_ENTRY PinViewListHead ; // offset 0x23c
    uint32_t FileObject ; // offset 0x244
    UNICODE_STRING FileFullPath ; // offset 0x248
    UNICODE_STRING FileUserName ; // offset 0x250
    uint16_t MappedViews ; // offset 0x258
    uint16_t PinnedViews ; // offset 0x25a
    uint32_t UseCount ; // offset 0x25c
    uint32_t SecurityCount ; // offset 0x260
    uint32_t SecurityCacheSize ; // offset 0x264
    int32_t SecurityHitHint ; // offset 0x268
    uint32_t SecurityCache ; // offset 0x26c
    LIST_ENTRY SecurityHash[64] ; // offset 0x270
    uint32_t UnloadEvent ; // offset 0x470
    uint32_t RootKcb ; // offset 0x474
    uint8_t Frozen ; // offset 0x478
    uint32_t UnloadWorkItem ; // offset 0x47c
    uint8_t GrowOnlyMode ; // offset 0x480
    uint32_t GrowOffset ; // offset 0x484
    LIST_ENTRY KcbConvertListHead ; // offset 0x488
    LIST_ENTRY KnodeConvertListHead ; // offset 0x490
    uint32_t CellRemapArray ; // offset 0x498

} CMHIVE, *PCMHIVE, **PPCMHIVE ;

