
class DLListNode {
    constructor(data) {
        this.data = data;
        this.prev = null;
        this.next = null;
    }
}

const head = Symbol("head of dllist")
const tail = Symbol("tail of dllist")

class DLList {
    constructor() {
        this[head] = null;
        this[tail] = null;

        this.size = 0;
    }

    add(data) {
        const newNode = new DLListNode(data);
        
        if(this[head] == null) {
            this[head] = newNode;
        } else {
            this[tail].next = newNode;
            newNode.prev = this[tail];
        }
        this[tail] = newNode;
        this.size++;
    }

    toString() {
        let str = "";
        let current = this[head];

        while(current != null) {
            str += "&tbsp;-&gt;{lno: "+current.lno+",data: \""+current.data+"\"}<br>";
            current = current.next;
        }
        return str;
    }
}