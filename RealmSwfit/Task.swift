//
//  Task.swift
//  RealmSwfit
//
//  Created by Jesus Mora on 23/3/22.
//

import Foundation
import RealmSwift

class Task: Object, ObjectKeyIdentifiable {
    @Persisted(primaryKey: true) var id: ObjectId
    @Persisted var title = ""
    @Persisted var completed = false    
}
