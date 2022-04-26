# Tagging FS, A modern way to organize storages

## About
This is a Filsystem that wants to "order" Files in Tags rather then Folders. Although the mounting would suggests that there are Folders, there aren't, Folders are just the way to represent the tags.

## Prerequisites

You need to have libfuse3 isntalled. \
This can be done via a Package manager or directly from the Git: https://github.com/libfuse/libfuse \

```
apt install libfuse3-dev
```
Or any other Package Manager that has the version 3 of libfuse. \

## Idea

```bash
mkdir Music
mkdir Documents
mkdir Video
> yeah.mp4
> nope.mp3
mv yeah.mp4 Video
mv nope.mp3 Music
cp Music/nope.mp3 Documents
cp Video/yeah.mp4 Documents
```

The above will Result in the following tree.

```
/
/Music
/Documents
/Video
/Music/nope.mp3
/Video/yeah.mp4
/Documents/nope.mp3
/Documents/yeah.mp4
```

But with the twist that: \
    /Music/nope.mp3 and /Documents/nope.mp3 are the same File (not just a Copy or a link, the exact same file that lies on the Device later on) \
    /Documents/Music/nope.mp3 is ALSO a valid path because the nope.mp3 file has now the tags Documents and Music. \
    So going into the Documents "folder" will result in the Music Folder being shown because some file, the yeah.mp4 file, has both thags! \
    This will basically filter files and tags to some degree! \
