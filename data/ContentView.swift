//
//  ContentView.swift
//  {{project_name}}
//
//  Created by Tiago Aguiar on {{date}}.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack {
            Image(systemName: "globe")
                .imageScale(.large)
                .foregroundColor(.accentColor)
            Text("Hello {{project_name}}")
              .font(Font.system(size: 30.0))
        }
        .padding()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
