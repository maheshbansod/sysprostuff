
class Buffer {
    constructor() {
        this.id = null; //identifier. e.g. dev no., block no. or line no.
        this.status = 'free';
        this.data = null; //data
        this.hqnext = null; //ptr to next buffer on hash queue
        this.hqprev = null; //ptr to prev buffer on hash queue
        this.flnext = null; //ptr to next buffer on free list
        this.flprev = null; //ptr to prev buffer on free list
    }
}

class BufferCache {
    constructor(hashfn, limit) {
        this.hashQ = []; //array of hash queues
        this.hash = hashfn; //hashing function .. e.g. mod 4
        this.limit = limit; //total no. of buffers in this cache
        this.freelist = {flnext: 0, flprev: 0}; //head of free list
        this.freelist.flnext = this.freelist;
        this.freelist.flprev = this.freelist;

        this.initBufferCache();
    }


    /**
     * get line no. `id` from file `file`
     * 
    */
    async getblk(file, id) {
        var done = false;
        var buf;
        while( !done ) {
            buf = this.findBuffer(id);
            if(buf != null) { //buffer in hash queue
                if(buf.status == 'busy') {
                    while(buf.status != 'free');
                    continue;
                }
                buf.status = 'busy';
                this.removeFromFreeList(buf);
                return buf;
            } else { //not in hash queue. ,`, needs to be brought in.
                if(this.freelist.flnext == this.freelist) {
                    //freelist is empty
                    while(this.freelist.flnext == this.freelist);
                    continue;
                }
                buf = this.freelist.flnext;
                this.removeFromFreeList(buf);
                if(buf.status == 'delayed write') {
                    ;///TODO: async write the buffer
                    continue;
                }
                this.removeFromHashQ(buf);
                buf.id = id;
                this.addToHashQ(buf);
                await this.read(file, buf);
                return buf;
            }
        }
    }

    /**
     * creates `limit` buffers with random data and puts them in appropriate lists/queues.
    */
    initBufferCache() {
        for(var i=0;i<this.limit;i++)
            this.addToFreeListHead(new Buffer());
    }

    addToFreeListHead(buffer) {
        this.freelist.flnext.flprev = buffer;
        buffer.flnext = this.freelist.flnext;
        this.freelist.flnext = buffer;
        buffer.flprev = this.freelist;
    }

    removeFromFreeList(buffer) {
        if(buffer.flnext == null || buffer.flprev == null)
            return;
        buffer.flprev.flnext = buffer.flnext;
        buffer.flnext.flprev = buffer.flprev;
        buffer.flprev = buffer.flnext = null;
    }

    findBuffer(id) {
        var i = this.hash(id);
        var current = this.hashQ[i];
        if(!current)
            return null;
        current = current.hqnext;
        while(current != null) {
            if(current.id == id)
                return current;
            current = current.hqnext;
        }
        return null;
    }

    addToHashQ(buffer) {
        if(buffer.id == null)
            return null;
        var i = this.hash(buffer.id);
        if(!this.hashQ[i]) {
            this.hashQ[i] = {hqnext: buffer, hqprev: null};
            buffer.hqnext = null;
        } else {
            if(this.hashQ[i].hqnext != null)
                this.hashQ[i].hqnext.hqprev = buffer;
            buffer.hqnext = this.hashQ[i].hqnext;
            this.hashQ[i].hqnext = buffer;
        }
        buffer.hqprev = this.hashQ[i];
    }

    removeFromHashQ(buffer) {
        if(buffer.hqprev == null)
            return;
        buffer.hqprev.hqnext = buffer.hqnext;
        if(buffer.hqnext != null)
            buffer.hqnext.hqprev = buffer.hqprev;
        buffer.hqnext = buffer.hqprev = null;
    }

    /**
     * reads line no. `buffer.id` from file and writes to buffer   
     */
    async read(file,buffer) {
        var lno = buffer.id;
        if(!file) return null;
        const data = await new Response(file).text();
        const lines = data.split(/\r\n|\n/);
        if(lno -1 >= lines.length || lno <= 0) {
            //alert("Couldn't fetch line number ",lno," from the file.");
            //this.addToFreeListHead(buffer);
            //buffer.status='free';
            buffer.data = "...";
        } else
            buffer.data = lines[lno-1]
    }

    isFull() {
        return (this.freelist.flnext == this.freelist);
    }

}