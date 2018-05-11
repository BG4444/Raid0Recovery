#include "xfsdetector.h"
#include <QUuid>
#include <QtEndian>
#include <QtWidgets/QPlainTextEdit>

typedef unsigned char uuid_t[16];
typedef uint64_t	xfs_rfsblock_t;	/* blockno in filesystem (raw) */
typedef uint64_t	xfs_fsblock_t;	/* blockno in filesystem (agno|agbno) */
typedef uint64_t	xfs_rtblock_t;	/* extent (block) in realtime area */
typedef uint64_t	xfs_ino_t;
typedef uint32_t	xfs_agblock_t;	/* blockno in alloc. group */
typedef uint32_t	xfs_extlen_t;	/* extent length in blocks */
typedef uint32_t	xfs_agnumber_t;	/* allocation group number */
typedef int64_t		xfs_lsn_t;	/* log sequence number */

#pragma pack(push,1)

typedef struct xfs_sb
{
    uint32_t	sb_magicnum;	/* magic number == XFS_SB_MAGIC */
    uint32_t	sb_blocksize;	/* logical block size, bytes */
    xfs_rfsblock_t	sb_dblocks;	/* number of data blocks */
    xfs_rfsblock_t	sb_rblocks;	/* number of realtime blocks */
    xfs_rtblock_t	sb_rextents;	/* number of realtime extents */
    uuid_t		sb_uuid;	/* user-visible file system unique id */
    xfs_fsblock_t	sb_logstart;	/* starting block of log if internal */
    xfs_ino_t	sb_rootino;	/* root inode number */
    xfs_ino_t	sb_rbmino;	/* bitmap inode for realtime extents */
    xfs_ino_t	sb_rsumino;	/* summary inode for rt bitmap */
    xfs_agblock_t	sb_rextsize;	/* realtime extent size, blocks */
    xfs_agblock_t	sb_agblocks;	/* size of an allocation group */
    xfs_agnumber_t	sb_agcount;	/* number of allocation groups */
    xfs_extlen_t	sb_rbmblocks;	/* number of rt bitmap blocks */
    xfs_extlen_t	sb_logblocks;	/* number of log blocks */
    uint16_t	sb_versionnum;	/* header version == XFS_SB_VERSION */
    uint16_t	sb_sectsize;	/* volume sector size, bytes */
    uint16_t	sb_inodesize;	/* inode size, bytes */
    uint16_t	sb_inopblock;	/* inodes per block */
    char		sb_fname[12];	/* file system name */
    uint8_t		sb_blocklog;	/* log2 of sb_blocksize */
    uint8_t		sb_sectlog;	/* log2 of sb_sectsize */
    uint8_t		sb_inodelog;	/* log2 of sb_inodesize */
    uint8_t		sb_inopblog;	/* log2 of sb_inopblock */
    uint8_t		sb_agblklog;	/* log2 of sb_agblocks (rounded up) */
    uint8_t		sb_rextslog;	/* log2 of sb_rextents */
    uint8_t		sb_inprogress;	/* mkfs is in progress, don't mount */
    uint8_t		sb_imax_pct;	/* max % of fs for inode space */
                    /* statistics */
    /*
     * These fields must remain contiguous.  If you really
     * want to change their layout, make sure you fix the
     * code in xfs_trans_apply_sb_deltas().
     */
    uint64_t	sb_icount;	/* allocated inodes */
    uint64_t	sb_ifree;	/* free inodes */
    uint64_t	sb_fdblocks;	/* free data blocks */
    uint64_t	sb_frextents;	/* free realtime extents */
    /*
     * End contiguous fields.
     */
    xfs_ino_t	sb_uquotino;	/* user quota inode */
    xfs_ino_t	sb_gquotino;	/* group quota inode */
    uint16_t	sb_qflags;	/* quota flags */
    uint8_t		sb_flags;	/* misc. flags */
    uint8_t		sb_shared_vn;	/* shared version number */
    xfs_extlen_t	sb_inoalignmt;	/* inode chunk alignment, fsblocks */
    uint32_t	sb_unit;	/* stripe or raid unit */
    uint32_t	sb_width;	/* stripe or raid width */
    uint8_t		sb_dirblklog;	/* log2 of dir block size (fsbs) */
    uint8_t		sb_logsectlog;	/* log2 of the log sector size */
    uint16_t	sb_logsectsize;	/* sector size for the log, bytes */
    uint32_t	sb_logsunit;	/* stripe unit size for the log */
    uint32_t	sb_features2;	/* additional feature bits */

    /*
     * bad features2 field as a result of failing to pad the sb structure to
     * 64 bits. Some machines will be using this field for features2 bits.
     * Easiest just to mark it bad and not use it for anything else.
     *
     * This is not kept up to date in memory; it is always overwritten by
     * the value in sb_features2 when formatting the incore superblock to
     * the disk buffer.
     */
    uint32_t	sb_bad_features2;

    /* version 5 superblock fields start here */

    /* feature masks */
    uint32_t	sb_features_compat;
    uint32_t	sb_features_ro_compat;
    uint32_t	sb_features_incompat;
    uint32_t	sb_features_log_incompat;

    uint32_t	sb_crc;		/* superblock crc */
    xfs_extlen_t	sb_spino_align;	/* sparse inode chunk alignment */

    xfs_ino_t	sb_pquotino;	/* project quota inode */
    xfs_lsn_t	sb_lsn;		/* last write sequence */
    uuid_t		sb_meta_uuid;	/* metadata file system unique id */

    /* must be padded to 64 bit alignment */
} xfs_sb_t;
#pragma pack(pop)

XFSDetector::XFSDetector(const SignatureDefInterface* parent):
                SignatureDetectorBase(parent,{'X','F','S','B',0,0,0x10,0})

{

}

void XFSDetector::build(QWidget *parent, const qulonglong stripeSize, const uchar *base, const quint64 size)
{
    QPlainTextEdit* edt=new QPlainTextEdit(parent);
//    xfs_sb* sb=reinterpret_cast<xfs_sb*>(base);

}


void XFSDetector::run(const std::atomic<bool> &stopper, uchar *base, const quint64 size, ProgressSignaler *sgn, const quint64 ofs)
{
    this->base=base;
    this->ofs=ofs;
    search(stopper,base,size,sgn,ofs);
}


void XFSDetector::onFound(const quint64 offset)
{
    const quint32 blockSize = qFromBigEndian(*reinterpret_cast<quint32*>( base+ (offset-ofs+4)  ));
    const quint64 fsSize = qFromBigEndian(*reinterpret_cast<quint64*>( base+ (offset-ofs+8)  ));

    const quint64 oneTb = quint64(1)<<(10*4);

    if(fsSize*blockSize > oneTb)
    {
        SignatureDetectorBase::onFound(offset);
    }
}

quint64 XFSDetector::granularity()
{
    return SignatureDetectorBase::granularity()+8;
}


QString XFSDetector::getDescription(const quint64 offset, QFile *file)
{
    if(offset+512>file->size())
    {
        return QString();
    }

    auto buf=file->map(offset,512);

    auto sb= reinterpret_cast<const xfs_sb*>(buf);

    const QByteArray uuid=QByteArray::fromRawData(reinterpret_cast<const char*>(sb->sb_uuid),sizeof(sb->sb_uuid)).toBase64();

    file->unmap(buf);

    return uuid;
}
