# Database File Sorting

Οδηγίες εκτέλεσης:

	Για την δημιουργία του εκτελέσιμου sort_main:
		make sort
    και για να το τρέξετε ./build/sort_main

    Για την δημιουργία του εκτελέσιμου του test:
		make chunk

	Για να τρέξετε τα tests:
		./build/test_chunk

Το πρόγραμμα αυτό είναι αποτέλεσμα μιας εργασίας στην οποία το μεγαλύτερο μέρος του κώδικα γράφτηκε με prompts του chatGPT.

Εδώ θα βρείτε ολόκληρη την αλληλεπίδραση μας με το chatGPT: https://chat.openai.com/share/5326e67d-53ca-4031-b6df-17756208a53d

Για την ταξινόμηση των συρμών χρησιμοποιούνται ο αλγόριθμος Merge Sort καθώς και οι συναρτήσεις compareRecords, shouldSwap 
οι οποίες ελέγχουν αν δύο εγγραφές πρέπει να αλλάξουν θέση στο αρχείο. 
Αρχικά, καλείται η sort_fileInChunks για την επαναληπτική διάσχιση όλων των συρμών και σε κάθε επανάληψη καλείται η sort_Chunk 
για να χειριστεί την σωστή ταξινόμηση του συγκεκριμένου chunk. 

Η συγχώνευση των ταξινομήμενων επιτυγχάνεται από τη συνάρτηση merge η οποία βασίζεται στον κώδικα που παράχθηκε από το chatGPT 
αλλά που χρειάστηκε διορθώσεις στο μεγαλύτερο βαθμό από όλα τα κομμάτια της εργασίας.
Αρχικά, δημιουργούμε έναν iterator για κάθε chunk και μέσα από ένα loop φορτώνουμε την πρώτη εγγραφή κάθε συρμού  
σε έναν πίνακα. Ύστερα, ταξινομούμε τις φορτωμένες εγγραφές και εισάγουμε την μικρότερη στο νέο αρχείο του Output. 
Συνεχίζουμε τη διαδικασία μέχρις ότου να μην υπάρχουν άλλες εγγραφές στο chunk για να πάρουν την θέση των ήδη εισηγμένων εγγραφών.
Όταν οι συνολικές εγγραφές του νέου αρχείου είναι ίσες με τις συνολικές εγγραφές του αρχικού αρχείου η συγχώνευση έχει ολοκληρωθεί επιτυχώς.
