#ifndef Recorder_H
#define Recorder_H
#include<QVector>
#include "recordbase.h"
#include<iostream>
#include "../../Helper/debug.h"
#include<QGraphicsItem>
#include<QObject>
#include <QPair>

class Recorder : public QObject
{
    Q_OBJECT
public:
    Recorder(QObject* parent=nullptr);
    ~Recorder();
    void record(RecordBase* record);
    void reset();
    void back();
    void forward();
    bool undoAvailiable();
    bool redoAvailiable();
signals:
    void recordChange();

private:
    struct RecordElement{
        RecordElement* next;
        RecordElement* before;
        RecordElement* siblingBefore;
        RecordElement* siblingNext;
        RecordBase* record;
        RecordElement(){
            next = NULL;
            before = NULL;
            record = NULL;
            siblingNext = NULL;
            siblingBefore = NULL;
        }
        RecordElement(RecordBase* record){
            next = NULL;
            before = NULL;
            this->record = record;
            siblingNext = NULL;
            siblingBefore = NULL;
        }
        RecordElement(RecordElement* before, RecordElement* next, RecordBase* record){
            this->before = before;
            this->next = next;
            this->record = record;
            siblingNext = NULL;
            siblingBefore = NULL;
        }
    };

    struct RecordStack{
        int limit;
        int index;
        RecordElement* root;
        RecordElement* current;
        QMap<int, QPair<RecordElement*, RecordElement*>> siblingMap;
        RecordStack()
            : limit(0),
              index(0){
            root = new RecordElement;
            root->next = root;
            root->before = root;
            current = root;
        }

        void reset(){
            RecordElement* iter = root->next
                ;
            while(iter->next != root){
                RecordElement* temp = iter;
                iter = iter->next;
                delete temp;
            }
            root->next = root;
            root->before = root;
            current = root;
            limit = 0;
            index = 0;
        }

        void free(){
            reset();
            delete root;
        }

        bool canUndo(){
            return index > 0;
        }

        bool canRedo(){
            return index < limit;
        }

        void push(RecordBase* record){
            if(current->next == root){
                RecordElement* recordElement = new RecordElement(current, current->next, record);
                current->next->before = recordElement;
                current->next = recordElement;
                addSibling(record, recordElement);
                current = recordElement;
            }
            else{
                current = current->next;
                removeSibling();
                addSibling(record, current);
                current->record->release();
                current->record = record;
            }
            index++;
            limit = index;
        }

        void forward(){
            if(!canRedo()) return;
            current = current->next;
            current->record->redo();
            index++;
        }

        void pop(){
            if(!canUndo()) return;
            current->record->undo();
            current = current->before;
            index--;
        }

        void addSibling(RecordBase* record, RecordElement* recordElement){
            recordElement->siblingNext = NULL;
            recordElement->siblingBefore = NULL;
            auto find = siblingMap.find(record->type);
            if(find != siblingMap.end()){
                find.value().second->siblingNext = recordElement;
                recordElement->siblingBefore = find.value().second;
            }
            else{
                siblingMap.insert(record->type, QPair<RecordElement*, RecordElement*>(recordElement, recordElement));
            }
        }

        void removeSibling(){
            auto find = siblingMap.find(current->record->type);
            if(find != siblingMap.end()){
                if(find.value().first == current){
                    RecordElement* tmp = current->siblingNext;
                    while(tmp != NULL){
                        tmp->before->next = tmp->next;
                        tmp->next->before = tmp->before;
                        RecordElement* tmp1 = tmp;
                        tmp = tmp->siblingNext;
                        delete tmp1;
                    }
                    siblingMap.remove(current->record->type);
                }
                else{
                    if(current == find.value().second){
                        siblingMap[current->record->type].second = current->siblingBefore;
                    }
                    if(current->siblingNext != NULL){
                        current->siblingNext->before = current->before;
                    }
                    current->siblingBefore->next = current->next;
                }
            }
        }

    };

private:
    RecordStack stack;
};

#endif // Recorder_H
