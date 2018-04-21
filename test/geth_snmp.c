int qeth_snmp_command(struct qeth_card *card, char __user *udata) { 
	struct qeth_cmd_buffer *iob; 
	struct qeth_ipa_cmd *cmd; 
	struct qeth_snmp_ureq *ureq; 
	int req_len; 
	struct qeth_arp_query_info qinfo = {0, }; 
	int rc = 0; 
	// [...] 10
	/* skip 4 bytes (data_len struct member) 11
	   to get req_len */
	if (copy_from_user(&req_len, udata 
				+ sizeof(int),sizeof(int))) 
		return -EFAULT; 

	ureq = memdup_user(udata, req_len + 
			sizeof(struct qeth_snmp_ureq_hdr));
	if (IS_ERR(ureq)) { 
		QETH_CARD_TEXT(card, 2, "snmpnome"); 
		return PTR_ERR(ureq); 
	} 
	// [...] 
	iob = qeth_get_adapter_cmd(card, 
			IPA_SETADP_SET_SNMP_CONTROL, 
			QETH_SNMP_SETADP_CMDLENGTH + req_len); 
	// [...] 27
	memcpy(&cmd->data.setadapterparms.data.snmp, 
			&ureq->cmd, req_len); 
	rc = qeth_send_ipa_snmp_cmd(card, iob, 
			QETH_SETADP_BASE_LEN + req_len, 
			qeth_snmp_command_cb, (void *)&qinfo); 
	// [...] 33
	kfree(ureq); 
	kfree(qinfo.udata); 
	return rc; 
}
